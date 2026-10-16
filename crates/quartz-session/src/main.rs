use clap::Parser;
use std::collections::HashMap;
use std::env;
use std::process::{Child, Command};
use std::sync::Arc;
use tokio::signal::unix::{signal, SignalKind};
use tokio::sync::Mutex;
use tracing::{info, warn};
use zbus::{connection, interface};

#[derive(Parser, Debug)]
#[command(author, version, about = "QuartzDE Wayland Session Starter")]
struct Args {
    #[arg(short, long, help = "Ejecutar en modo simulación (dry-run) sin lanzar KWin")]
    dry_run: bool,

    #[arg(long, default_value = "kwin_wayland", help = "Binario del compositor Wayland")]
    compositor: String,
}

#[derive(Debug, Clone)]
pub struct AppLifecycleState {
    pub app_id: String,
    pub name: String,
    pub open_windows: u32,
    pub is_resident: bool,
}

#[derive(Clone)]
pub struct SessionLifecycleManager {
    apps: Arc<Mutex<HashMap<String, AppLifecycleState>>>,
}

impl SessionLifecycleManager {
    pub fn new() -> Self {
        Self {
            apps: Arc::new(Mutex::new(HashMap::new())),
        }
    }
}

#[interface(name = "org.quartz.Session")]
impl SessionLifecycleManager {
    /// Invocado cuando se abre una nueva ventana de una aplicación
    async fn on_window_opened(&mut self, app_id: String, name: String) -> zbus::fdo::Result<()> {
        let mut map = self.apps.lock().await;
        let entry = map.entry(app_id.clone()).or_insert_with(|| AppLifecycleState {
            app_id,
            name,
            open_windows: 0,
            is_resident: true,
        });
        entry.open_windows += 1;
        info!("Ventana abierta para '{}'. Ventanas activas: {}", entry.name, entry.open_windows);
        Ok(())
    }

    /// Invocado con Ctrl+W: cierra la ventana pero mantiene el proceso residente
    async fn on_window_closed(&mut self, app_id: String) -> zbus::fdo::Result<bool> {
        let mut map = self.apps.lock().await;
        if let Some(entry) = map.get_mut(&app_id) {
            if entry.open_windows > 0 {
                entry.open_windows -= 1;
            }
            info!(
                "Ventana cerrada (Ctrl+W) para '{}'. Ventanas restantes: {}. Aplicación residente: {}",
                entry.name, entry.open_windows, entry.is_resident
            );
            return Ok(entry.is_resident);
        }
        Ok(false)
    }

    /// Invocado con Ctrl+Q: finaliza explícitamente el proceso de la aplicación
    async fn quit_application(&mut self, app_id: String) -> zbus::fdo::Result<()> {
        let mut map = self.apps.lock().await;
        if let Some(entry) = map.remove(&app_id) {
            info!("Terminación explícita (Ctrl+Q) solicitada para '{}'. Proceso liberado.", entry.name);
        }
        Ok(())
    }

    /// Consulta si la aplicación tiene indicador activo en el Dock
    async fn is_app_running(&self, app_id: String) -> zbus::fdo::Result<bool> {
        let map = self.apps.lock().await;
        Ok(map.contains_key(&app_id))
    }

    // --- Métodos de Control de Sesión y Energía ---
    async fn lock_screen(&self) -> zbus::fdo::Result<()> {
        info!("Solicitud de bloqueo de pantalla recibida vía D-Bus.");
        let _ = Command::new("loginctl").arg("lock-session").spawn();
        Ok(())
    }

    async fn request_logout(&self) -> zbus::fdo::Result<()> {
        info!("Solicitud de cierre de sesión recibida vía D-Bus.");
        let _ = Command::new("loginctl").arg("terminate-session").arg("").spawn();
        Ok(())
    }

    async fn request_reboot(&self) -> zbus::fdo::Result<()> {
        info!("Solicitud de reinicio de equipo recibida vía D-Bus.");
        let _ = Command::new("systemctl").arg("reboot").spawn();
        Ok(())
    }

    async fn request_shutdown(&self) -> zbus::fdo::Result<()> {
        info!("Solicitud de apagado de equipo recibida vía D-Bus.");
        let _ = Command::new("systemctl").arg("poweroff").spawn();
        Ok(())
    }

    async fn is_locked(&self) -> zbus::fdo::Result<bool> {
        Ok(false)
    }
}

use libquartz::config::{AppearanceMode, QuartzDesktopConfig};

#[derive(Clone)]
pub struct ConfigManager {
    config: Arc<Mutex<QuartzDesktopConfig>>,
}

impl ConfigManager {
    pub fn new() -> Self {
        let loaded = QuartzDesktopConfig::load_from_xdg();
        Self {
            config: Arc::new(Mutex::new(loaded)),
        }
    }
}

#[interface(name = "org.quartz.Config")]
impl ConfigManager {
    async fn get_config(&self) -> zbus::fdo::Result<String> {
        let cfg = self.config.lock().await;
        cfg.to_json().map_err(|e| zbus::fdo::Error::Failed(e.to_string()))
    }

    async fn set_config(&mut self, json: String, #[zbus(signal_context)] ctxt: zbus::SignalContext<'_>) -> zbus::fdo::Result<bool> {
        if let Ok(new_cfg) = QuartzDesktopConfig::from_json(&json) {
            let mut cfg = self.config.lock().await;
            *cfg = new_cfg.clone();
            let _ = cfg.save_to_xdg();
            let _ = Self::config_changed(&ctxt, &json).await;
            info!("Configuración de QuartzDE actualizada vía D-Bus.");
            return Ok(true);
        }
        Ok(false)
    }

    async fn get_stage_manager_enabled(&self) -> zbus::fdo::Result<bool> {
        let cfg = self.config.lock().await;
        Ok(cfg.stage_manager.enabled)
    }

    async fn set_stage_manager_enabled(&mut self, enabled: bool, #[zbus(signal_context)] ctxt: zbus::SignalContext<'_>) -> zbus::fdo::Result<bool> {
        let mut cfg = self.config.lock().await;
        if cfg.stage_manager.enabled != enabled {
            cfg.stage_manager.enabled = enabled;
            let _ = cfg.save_to_xdg();
            if let Ok(json) = cfg.to_json() {
                let _ = Self::config_changed(&ctxt, &json).await;
            }
            info!("Ajuste Stage Manager modificado: enabled={}", enabled);
        }
        Ok(true)
    }

    async fn get_appearance_mode(&self) -> zbus::fdo::Result<String> {
        let cfg = self.config.lock().await;
        let mode_str = match cfg.appearance {
            AppearanceMode::Light => "Light",
            AppearanceMode::Dark => "Dark",
            AppearanceMode::Auto => "Auto",
        };
        Ok(mode_str.to_string())
    }

    async fn set_appearance_mode(&mut self, mode: String, #[zbus(signal_context)] ctxt: zbus::SignalContext<'_>) -> zbus::fdo::Result<bool> {
        let mut cfg = self.config.lock().await;
        let new_mode = match mode.to_lowercase().as_str() {
            "light" => AppearanceMode::Light,
            "dark" => AppearanceMode::Dark,
            _ => AppearanceMode::Auto,
        };
        cfg.appearance = new_mode;
        let _ = cfg.save_to_xdg();
        if let Ok(json) = cfg.to_json() {
            let _ = Self::config_changed(&ctxt, &json).await;
        }
        info!("Ajuste Modo de Apariencia modificado: {:?}", new_mode);
        Ok(true)
    }

    async fn get_theme(&self) -> zbus::fdo::Result<String> {
        let cfg = self.config.lock().await;
        Ok(cfg.theme_name.clone())
    }

    async fn set_theme(&mut self, theme: String, #[zbus(signal_context)] ctxt: zbus::SignalContext<'_>) -> zbus::fdo::Result<bool> {
        let mut cfg = self.config.lock().await;
        cfg.theme_name = theme.clone();
        let _ = cfg.save_to_xdg();
        if let Ok(json) = cfg.to_json() {
            let _ = Self::config_changed(&ctxt, &json).await;
        }
        info!("Tema QuartzDE actualizado a: {}", theme);
        Ok(true)
    }

    async fn get_icon_theme(&self) -> zbus::fdo::Result<String> {
        let cfg = self.config.lock().await;
        Ok(cfg.icon_theme.clone())
    }

    async fn set_icon_theme(&mut self, icon_theme: String, #[zbus(signal_context)] ctxt: zbus::SignalContext<'_>) -> zbus::fdo::Result<bool> {
        let mut cfg = self.config.lock().await;
        cfg.icon_theme = icon_theme.clone();
        let _ = cfg.save_to_xdg();
        if let Ok(json) = cfg.to_json() {
            let _ = Self::config_changed(&ctxt, &json).await;
        }
        info!("Tema de iconos QuartzDE actualizado a: {}", icon_theme);
        Ok(true)
    }

    #[zbus(signal)]
    async fn config_changed(ctxt: &zbus::SignalContext<'_>, json: &str) -> zbus::Result<()>;
}

use std::path::{Path, PathBuf};
use std::time::Duration;

pub async fn wait_for_wayland_socket(
    socket_path: &Path,
    timeout_duration: Duration,
    mut compositor: Option<&mut Child>,
) -> Result<(), Box<dyn std::error::Error>> {
    let start = std::time::Instant::now();
    let poll_interval = Duration::from_millis(50);

    while start.elapsed() < timeout_duration {
        if socket_path.exists() {
            info!("Socket Wayland detectado en {:?}", socket_path);
            return Ok(());
        }

        if let Some(child) = compositor.as_deref_mut() {
            if let Ok(Some(status)) = child.try_wait() {
                return Err(format!("El compositor Wayland terminó prematuramente con estado: {}", status).into());
            }
        }

        tokio::time::sleep(poll_interval).await;
    }

    Err(format!(
        "Tiempo de espera agotado ({:?}) esperando el socket Wayland en {:?}",
        timeout_duration, socket_path
    )
    .into())
}

struct SessionManager {
    children: Vec<Child>,
    compositor: Option<Child>,
}

impl SessionManager {
    fn new() -> Self {
        Self {
            children: Vec::new(),
            compositor: None,
        }
    }

    fn setup_environment() {
        info!("Configurando variables de entorno para sesión QuartzDE Wayland...");
        env::set_var("XDG_CURRENT_DESKTOP", "Quartz:KDE");
        env::set_var("XDG_SESSION_DESKTOP", "Quartz");
        env::set_var("XDG_SESSION_TYPE", "wayland");
        env::set_var("QT_QPA_PLATFORM", "wayland");
        env::set_var("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");
        env::set_var("GDK_BACKEND", "wayland");
        env::set_var("KDE_FULL_SESSION", "true");
        env::set_var("KDE_SESSION_VERSION", "6");
        env::set_var("XCURSOR_THEME", "Colloid-cursors");
        env::set_var("XCURSOR_SIZE", "24");
        env::set_var("QT_QPA_PLATFORMTHEME", "kde");
    }

    fn spawn_compositor(&mut self, cmd: &str, socket_name: &str) -> Result<PathBuf, Box<dyn std::error::Error>> {
        info!("Iniciando compositor Wayland principal: {} (socket: {})", cmd, socket_name);
        let runtime_dir = env::var("XDG_RUNTIME_DIR")
            .map(PathBuf::from)
            .unwrap_or_else(|_| PathBuf::from(format!("/run/user/{}", nix::unistd::getuid())));
        let socket_path = runtime_dir.join(socket_name);

        if socket_path.exists() {
            let _ = std::fs::remove_file(&socket_path);
        }

        let child = Command::new(cmd)
            .args(["--xwayland", "--no-lockscreen", "--socket", socket_name])
            .spawn()
            .map_err(|e| format!("No se pudo ejecutar el compositor '{}': {}", cmd, e))?;

        self.compositor = Some(child);
        Ok(socket_path)
    }

    fn spawn_service(&mut self, cmd: &str, args: &[&str]) {
        info!("Lanzando servicio de sesión: {} {:?}", cmd, args);
        match Command::new(cmd).args(args).spawn() {
            Ok(child) => {
                self.children.push(child);
            }
            Err(e) => {
                warn!("No se pudo iniciar {}: {}. Continuando...", cmd, e);
            }
        }
    }

    fn terminate_all(&mut self) {
        info!("Finalizando servicios de sesión...");
        for child in &mut self.children {
            let _ = child.kill();
        }
        if let Some(mut comp) = self.compositor.take() {
            info!("Finalizando compositor Wayland...");
            let _ = comp.kill();
        }
    }
}

impl Drop for SessionManager {
    fn drop(&mut self) {
        self.terminate_all();
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    tracing_subscriber::fmt::init();
    let args = Args::parse();

    info!("Iniciando QuartzDE Session Manager (v0.1.0)...");
    SessionManager::setup_environment();

    if args.dry_run {
        info!("Modo dry-run activado: variables configuradas correctamente. Saliendo sin lanzar procesos.");
        return Ok(());
    }

    let mut session = SessionManager::new();

    // 1. Asegurar compositor Wayland
    let wayland_display = env::var("WAYLAND_DISPLAY").ok();
    if let Some(active_display) = wayland_display {
        info!("Sesión Wayland preexistente detectada (WAYLAND_DISPLAY={}). Omitiendo lanzamiento de compositor.", active_display);
    } else {
        let socket_name = "wayland-0";
        let socket_path = session.spawn_compositor(&args.compositor, socket_name)?;
        info!("Esperando disponibilidad del socket Wayland en {:?}...", socket_path);
        wait_for_wayland_socket(&socket_path, Duration::from_secs(5), session.compositor.as_mut()).await?;
        env::set_var("WAYLAND_DISPLAY", socket_name);
        env::set_var("DISPLAY", ":0");
        info!("Compositor Wayland inicializado y listo en {}", socket_name);
    }

    // 2. Registrar servicios de sesión y configuración en D-Bus
    let lifecycle = SessionLifecycleManager::new();
    let config_mgr = ConfigManager::new();
    let _conn = match connection::Builder::session() {
        Ok(builder) => {
            match builder
                .name("org.quartz.Session")
                .and_then(|b| b.name("org.quartz.Config"))
                .and_then(|b| b.serve_at("/org/quartz/Session", lifecycle))
                .and_then(|b| b.serve_at("/org/quartz/Config", config_mgr))
            {
                Ok(b) => match b.build().await {
                    Ok(conn) => {
                        info!("Servicios D-Bus org.quartz.Session y org.quartz.Config registrados correctamente.");
                        Some(conn)
                    }
                    Err(e) => {
                        warn!("No se pudo inicializar la conexión D-Bus: {}. Continuando...", e);
                        None
                    }
                },
                Err(e) => {
                    warn!("Error configurando interfaces D-Bus: {}. Continuando...", e);
                    None
                }
            }
        }
        Err(e) => {
            warn!("No se pudo conectar al bus de sesión D-Bus: {}. Continuando...", e);
            None
        }
    };

    // 3. Lanzar servicios del entorno de escritorio
    session.spawn_service("quartz-quicklookd", &[]);
    session.spawn_service("quartz-spotlight", &[]);
    session.spawn_service("quartz-shell", &[]);

    let mut sigterm = signal(SignalKind::terminate())?;
    let mut sigint = signal(SignalKind::interrupt())?;

    info!("Sesión QuartzDE activa y gestionando ciclo de vida de documentos y procesos.");

    let mut check_interval = tokio::time::interval(Duration::from_millis(250));

    loop {
        tokio::select! {
            _ = sigterm.recv() => {
                info!("Recibida señal SIGTERM. Cerrando sesión ordenadamente...");
                break;
            }
            _ = sigint.recv() => {
                info!("Recibida señal SIGINT (Ctrl+C). Cerrando sesión...");
                break;
            }
            _ = check_interval.tick() => {
                if let Some(child) = session.compositor.as_mut() {
                    match child.try_wait() {
                        Ok(Some(status)) => {
                            info!("El compositor Wayland ha finalizado con estado {}. Cerrando sesión QuartzDE...", status);
                            break;
                        }
                        Ok(None) => {}
                        Err(e) => {
                            warn!("Error al consultar estado del compositor: {}", e);
                        }
                    }
                }
            }
        }
    }

    session.terminate_all();
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_environment_variables() {
        SessionManager::setup_environment();
        assert_eq!(env::var("XDG_CURRENT_DESKTOP").unwrap(), "Quartz:KDE");
        assert_eq!(env::var("QT_QPA_PLATFORM").unwrap(), "wayland");
        assert_eq!(env::var("XCURSOR_THEME").unwrap(), "Colloid-cursors");
        assert_eq!(env::var("XCURSOR_SIZE").unwrap(), "24");
        assert_eq!(env::var("QT_QPA_PLATFORMTHEME").unwrap(), "kde");
    }

    #[tokio::test]
    async fn test_app_lifecycle_retention() {
        let mut manager = SessionLifecycleManager::new();
        manager.on_window_opened("quartz-finder".into(), "Finder".into()).await.unwrap();
        assert!(manager.is_app_running("quartz-finder".into()).await.unwrap());

        // Ctrl+W: Cierra ventana, pero sigue residente
        let is_resident = manager.on_window_closed("quartz-finder".into()).await.unwrap();
        assert!(is_resident);
        assert!(manager.is_app_running("quartz-finder".into()).await.unwrap());

        // Ctrl+Q: Salida definitiva
        manager.quit_application("quartz-finder".into()).await.unwrap();
        assert!(!manager.is_app_running("quartz-finder".into()).await.unwrap());
    }

    #[tokio::test]
    async fn test_wait_for_wayland_socket_success() {
        let temp_dir = std::env::temp_dir();
        let socket_path = temp_dir.join(format!("quartz_test_socket_{}", std::process::id()));
        let _ = std::fs::File::create(&socket_path).unwrap();

        let res = wait_for_wayland_socket(&socket_path, Duration::from_millis(200), None).await;
        let _ = std::fs::remove_file(&socket_path);
        assert!(res.is_ok());
    }

    #[tokio::test]
    async fn test_wait_for_wayland_socket_timeout() {
        let temp_dir = std::env::temp_dir();
        let socket_path = temp_dir.join(format!("quartz_non_existent_{}", std::process::id()));
        let res = wait_for_wayland_socket(&socket_path, Duration::from_millis(100), None).await;
        assert!(res.is_err());
    }

    #[tokio::test]
    async fn test_session_power_methods() {
        let manager = SessionLifecycleManager::new();
        assert!(!manager.is_locked().await.unwrap());
    }

    #[tokio::test]
    async fn test_config_manager_methods() {
        let manager = ConfigManager::new();
        let cfg_json = manager.get_config().await.unwrap();
        assert!(cfg_json.contains("Aqua"));
        assert!(cfg_json.contains("Colloid"));

        assert_eq!(manager.get_theme().await.unwrap(), "Aqua");
        assert_eq!(manager.get_icon_theme().await.unwrap(), "Colloid");
        assert_eq!(manager.get_appearance_mode().await.unwrap(), "Auto");
    }
}
