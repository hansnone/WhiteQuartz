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

    // 2. Registrar servicio de ciclo de vida de aplicaciones en D-Bus
    let lifecycle = SessionLifecycleManager::new();
    match connection::Builder::session() {
        Ok(builder) => {
            match builder
                .name("org.quartz.Session")
                .and_then(|b| b.serve_at("/org/quartz/Session", lifecycle))
            {
                Ok(b) => match b.build().await {
                    Ok(_conn) => {
                        info!("Servicio D-Bus org.quartz.Session registrado para ciclo de vida de aplicaciones.");
                    }
                    Err(e) => {
                        warn!("No se pudo inicializar la conexión D-Bus: {}. Continuando...", e);
                    }
                },
                Err(e) => {
                    warn!("Error configurando interfaz D-Bus: {}. Continuando...", e);
                }
            }
        }
        Err(e) => {
            warn!("No se pudo conectar al bus de sesión D-Bus: {}. Continuando...", e);
        }
    }

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
}
