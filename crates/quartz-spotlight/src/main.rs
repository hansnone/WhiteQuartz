use clap::Parser;
use std::fs;
use std::path::{Path, PathBuf};
use std::sync::OnceLock;
use tracing::info;
use zbus::{connection, interface};

#[derive(Parser, Debug)]
#[command(author, version, about = "QuartzDE Spotlight Search Service")]
struct Args {
    #[arg(short, long, help = "Ejecutar consulta única desde línea de comandos")]
    query: Option<String>,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum SearchResultCategory {
    Calculator,
    Application,
    File,
    Setting,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct SearchResultItem {
    pub title: String,
    pub subtitle: String,
    pub category: SearchResultCategory,
    pub icon_name: String,
    pub action_payload: String,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct DesktopAppEntry {
    pub name: String,
    pub generic_name: String,
    pub comment: String,
    pub exec: String,
    pub icon: String,
}

impl DesktopAppEntry {
    pub fn parse_string(content: &str) -> Option<Self> {
        let mut in_desktop_entry = false;
        let mut name = None;
        let mut generic_name = None;
        let mut comment = None;
        let mut exec = None;
        let mut icon = None;
        let mut no_display = false;

        for line in content.lines() {
            let line = line.trim();
            if line.starts_with('[') {
                in_desktop_entry = line == "[Desktop Entry]";
                continue;
            }
            if !in_desktop_entry {
                continue;
            }
            if let Some((k, v)) = line.split_once('=') {
                let key = k.trim();
                let val = v.trim();
                match key {
                    "Name" if name.is_none() => name = Some(val.to_string()),
                    "GenericName" if generic_name.is_none() => generic_name = Some(val.to_string()),
                    "Comment" if comment.is_none() => comment = Some(val.to_string()),
                    "Icon" if icon.is_none() => icon = Some(val.to_string()),
                    "NoDisplay" => {
                        if val.eq_ignore_ascii_case("true") {
                            no_display = true;
                        }
                    }
                    "Exec" if exec.is_none() => {
                        let cleaned: Vec<&str> = val
                            .split_whitespace()
                            .filter(|tok| !tok.starts_with('%'))
                            .collect();
                        exec = Some(cleaned.join(" "));
                    }
                    _ => {}
                }
            }
        }

        if no_display {
            return None;
        }

        let app_name = name?;
        let app_exec = exec.unwrap_or_else(|| app_name.to_lowercase());

        Some(Self {
            name: app_name,
            generic_name: generic_name.unwrap_or_default(),
            comment: comment.unwrap_or_default(),
            exec: app_exec,
            icon: icon.unwrap_or_else(|| "application-x-executable".to_string()),
        })
    }

    pub fn parse_file(path: &Path) -> Option<Self> {
        let content = fs::read_to_string(path).ok()?;
        Self::parse_string(&content)
    }
}

pub fn scan_applications() -> Vec<DesktopAppEntry> {
    let mut apps = Vec::new();
    let mut search_dirs = vec![
        PathBuf::from("/usr/share/applications"),
        PathBuf::from("/usr/local/share/applications"),
    ];
    if let Ok(home) = std::env::var("HOME") {
        search_dirs.push(PathBuf::from(home).join(".local/share/applications"));
    }

    for dir in search_dirs {
        if let Ok(entries) = fs::read_dir(dir) {
            for entry in entries.flatten() {
                let path = entry.path();
                if path.extension().and_then(|s| s.to_str()) == Some("desktop") {
                    if let Some(app) = DesktopAppEntry::parse_file(&path) {
                        apps.push(app);
                    }
                }
            }
        }
    }
    apps
}

pub fn search_files_in_home(query: &str, max_results: usize) -> Vec<SearchResultItem> {
    let mut results = Vec::new();
    let home = match std::env::var("HOME") {
        Ok(h) => PathBuf::from(h),
        Err(_) => return results,
    };
    let query_lower = query.to_lowercase();

    fn walk(
        dir: &Path,
        depth: usize,
        query: &str,
        results: &mut Vec<SearchResultItem>,
        max: usize,
    ) {
        if depth > 3 || results.len() >= max {
            return;
        }
        let entries = match fs::read_dir(dir) {
            Ok(e) => e,
            Err(_) => return,
        };
        for entry in entries.flatten() {
            if results.len() >= max {
                break;
            }
            let file_name = entry.file_name();
            let name_str = file_name.to_string_lossy();
            if name_str.starts_with('.')
                || name_str == "target"
                || name_str == "node_modules"
                || name_str == "build"
            {
                continue;
            }
            let path = entry.path();
            if name_str.to_lowercase().contains(query) {
                let is_dir = path.is_dir();
                results.push(SearchResultItem {
                    title: name_str.to_string(),
                    subtitle: path.to_string_lossy().to_string(),
                    category: SearchResultCategory::File,
                    icon_name: if is_dir {
                        "folder".to_string()
                    } else {
                        "document".to_string()
                    },
                    action_payload: path.to_string_lossy().to_string(),
                });
            }
            if path.is_dir() {
                walk(&path, depth + 1, query, results, max);
            }
        }
    }

    walk(&home, 1, &query_lower, &mut results, max_results);
    results
}

static CACHED_APPS: OnceLock<Vec<DesktopAppEntry>> = OnceLock::new();

pub struct SpotlightEngine;

impl SpotlightEngine {
    pub fn get_apps() -> &'static [DesktopAppEntry] {
        CACHED_APPS.get_or_init(scan_applications)
    }

    pub fn evaluate_math(input: &str) -> Option<f64> {
        let clean = input.trim().replace(' ', "");
        if clean.is_empty() || !clean.chars().all(|c| c.is_ascii_digit() || "+-*/.".contains(c)) {
            return None;
        }

        // Evaluador aritmético seguro y sencillo para sumas, restas, multiplicaciones y divisiones
        if let Some((left, right)) = clean.split_once('+') {
            let l = left.parse::<f64>().ok()?;
            let r = right.parse::<f64>().ok()?;
            return Some(l + r);
        } else if let Some((left, right)) = clean.split_once('-') {
            let l = left.parse::<f64>().ok()?;
            let r = right.parse::<f64>().ok()?;
            return Some(l - r);
        } else if let Some((left, right)) = clean.split_once('*') {
            let l = left.parse::<f64>().ok()?;
            let r = right.parse::<f64>().ok()?;
            return Some(l * r);
        } else if let Some((left, right)) = clean.split_once('/') {
            let l = left.parse::<f64>().ok()?;
            let r = right.parse::<f64>().ok()?;
            if r.abs() < f64::EPSILON {
                return None;
            }
            return Some(l / r);
        }

        None
    }

    pub fn query(text: &str) -> Vec<SearchResultItem> {
        let mut results = Vec::new();
        let trimmed = text.trim();

        if trimmed.is_empty() {
            return results;
        }

        // 1. Comprobar si es operación matemática
        if let Some(val) = Self::evaluate_math(trimmed) {
            results.push(SearchResultItem {
                title: format!("{val}"),
                subtitle: format!("Cálculo: {trimmed}"),
                category: SearchResultCategory::Calculator,
                icon_name: "accessories-calculator".to_string(),
                action_payload: format!("{val}"),
            });
        }

        let query_lower = trimmed.to_lowercase();

        // 2. Coincidencia con aplicaciones escaneadas del sistema
        let apps = Self::get_apps();
        for app in apps {
            if app.name.to_lowercase().contains(&query_lower)
                || app.generic_name.to_lowercase().contains(&query_lower)
                || app.comment.to_lowercase().contains(&query_lower)
            {
                let subtitle = if !app.comment.is_empty() {
                    app.comment.clone()
                } else if !app.generic_name.is_empty() {
                    app.generic_name.clone()
                } else {
                    app.exec.clone()
                };
                results.push(SearchResultItem {
                    title: app.name.clone(),
                    subtitle,
                    category: SearchResultCategory::Application,
                    icon_name: app.icon.clone(),
                    action_payload: app.exec.clone(),
                });
            }
        }

        // Aplicaciones Quartz integradas (fallback y garantías de paridad)
        let built_in_apps = [
            ("Ajustes del Sistema", "quartz-settings", "Configuración y preferencias de QuartzDE", "preferences-system"),
            ("Finder", "quartz-finder", "Gestor de archivos nativo", "system-file-manager"),
            ("Terminal", "x-terminal-emulator", "Emulador de terminal del sistema", "utilities-terminal"),
            ("Monitor de Actividad", "plasma-systemmonitor", "Rendimiento y procesos", "utilities-system-monitor"),
        ];

        for (name, exec, desc, icon) in built_in_apps {
            if name.to_lowercase().contains(&query_lower) {
                // Evitar duplicar si ya vino de /usr/share/applications
                if !results.iter().any(|r| r.title.eq_ignore_ascii_case(name)) {
                    results.push(SearchResultItem {
                        title: name.to_string(),
                        subtitle: desc.to_string(),
                        category: SearchResultCategory::Application,
                        icon_name: icon.to_string(),
                        action_payload: exec.to_string(),
                    });
                }
            }
        }

        // 3. Búsqueda de archivos en $HOME (si la consulta tiene al menos 2 caracteres)
        if trimmed.len() >= 2 {
            let files = search_files_in_home(trimmed, 6);
            results.extend(files);
        }

        results
    }
}

pub struct SpotlightServer;

#[interface(name = "org.quartz.Spotlight")]
impl SpotlightServer {
    async fn toggle(&self, #[zbus(signal_context)] ctxt: zbus::SignalContext<'_>) -> zbus::fdo::Result<()> {
        info!("Alternando visibilidad de la barra flotante de Spotlight (680px)");
        let _ = Self::toggled(&ctxt).await;
        Ok(())
    }

    async fn search(&self, text: String) -> zbus::fdo::Result<Vec<(String, String, String, String, String)>> {
        let items = SpotlightEngine::query(&text);
        let serializable = items
            .into_iter()
            .map(|i| {
                let cat_str = match i.category {
                    SearchResultCategory::Calculator => "Calculation",
                    SearchResultCategory::Application => "App",
                    SearchResultCategory::File => "File",
                    SearchResultCategory::Setting => "Setting",
                };
                (i.title, i.subtitle, cat_str.to_string(), i.icon_name, i.action_payload)
            })
            .collect();
        Ok(serializable)
    }

    async fn activate_item(&self, payload: String) -> zbus::fdo::Result<bool> {
        info!("Activando elemento de Spotlight con payload: {}", payload);
        let trimmed = payload.trim();
        if trimmed.is_empty() {
            return Ok(false);
        }
        let parts: Vec<&str> = trimmed.split_whitespace().collect();
        if parts.is_empty() {
            return Ok(false);
        }
        let program = parts[0];
        let args = &parts[1..];
        let res = std::process::Command::new(program).args(args).spawn();
        Ok(res.is_ok())
    }

    #[zbus(signal)]
    async fn toggled(ctxt: &zbus::SignalContext<'_>) -> zbus::Result<()>;
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    tracing_subscriber::fmt::init();
    let args = Args::parse();

    if let Some(q) = args.query {
        let results = SpotlightEngine::query(&q);
        println!("Resultados de Spotlight para '{}':", q);
        for item in results {
            println!("  [{:?}] {} - {}", item.category, item.title, item.subtitle);
        }
        return Ok(());
    }

    info!("Iniciando servicio de búsqueda Spotlight en Rust (org.quartz.Spotlight)...");

    let server = SpotlightServer;
    let _conn = connection::Builder::session()?
        .name("org.quartz.Spotlight")?
        .serve_at("/org/quartz/Spotlight", server)?
        .build()
        .await?;

    info!("quartz-spotlight listo y escuchando en D-Bus.");
    std::future::pending::<()>().await;
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_math_evaluation() {
        assert_eq!(SpotlightEngine::evaluate_math("25 + 75"), Some(100.0));
        assert_eq!(SpotlightEngine::evaluate_math("10 * 5"), Some(50.0));
        assert_eq!(SpotlightEngine::evaluate_math("100 / 4"), Some(25.0));
        assert_eq!(SpotlightEngine::evaluate_math("80 - 30"), Some(50.0));
        assert_eq!(SpotlightEngine::evaluate_math("invalid"), None);
    }

    #[test]
    fn test_desktop_entry_parsing() {
        let content = r#"
[Desktop Entry]
Type=Application
Name=Text Editor
GenericName=Editor de Texto
Comment=Edita archivos de texto
Exec=gedit %U
Icon=org.gnome.gedit
NoDisplay=false
"#;
        let entry = DesktopAppEntry::parse_string(content).expect("Debe parsear Desktop Entry");
        assert_eq!(entry.name, "Text Editor");
        assert_eq!(entry.generic_name, "Editor de Texto");
        assert_eq!(entry.exec, "gedit");
        assert_eq!(entry.icon, "org.gnome.gedit");
    }

    #[test]
    fn test_desktop_entry_nodisplay() {
        let content = r#"
[Desktop Entry]
Type=Application
Name=Hidden App
Exec=hidden
NoDisplay=true
"#;
        let entry = DesktopAppEntry::parse_string(content);
        assert!(entry.is_none(), "Entrada con NoDisplay=true debe ser ignorada");
    }

    #[test]
    fn test_app_query() {
        let results = SpotlightEngine::query("Ajustes");
        assert!(!results.is_empty());
        assert_eq!(results[0].title, "Ajustes del Sistema");
        assert_eq!(results[0].category, SearchResultCategory::Application);
    }
}
