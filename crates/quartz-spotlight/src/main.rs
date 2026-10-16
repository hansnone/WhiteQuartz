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
        let clean = input.trim();
        if clean.is_empty() {
            return None;
        }

        struct Parser<'a> {
            chars: std::iter::Peekable<std::str::Chars<'a>>,
        }

        impl<'a> Parser<'a> {
            fn new(s: &'a str) -> Self {
                Self {
                    chars: s.chars().peekable(),
                }
            }

            fn skip_whitespace(&mut self) {
                while let Some(&c) = self.chars.peek() {
                    if c.is_whitespace() {
                        self.chars.next();
                    } else {
                        break;
                    }
                }
            }

            fn parse_expr(&mut self) -> Option<f64> {
                let mut val = self.parse_term()?;
                loop {
                    self.skip_whitespace();
                    match self.chars.peek() {
                        Some('+') => {
                            self.chars.next();
                            let rhs = self.parse_term()?;
                            val += rhs;
                        }
                        Some('-') => {
                            self.chars.next();
                            let rhs = self.parse_term()?;
                            val -= rhs;
                        }
                        _ => break,
                    }
                }
                Some(val)
            }

            fn parse_term(&mut self) -> Option<f64> {
                let mut val = self.parse_factor()?;
                loop {
                    self.skip_whitespace();
                    match self.chars.peek() {
                        Some('*') => {
                            self.chars.next();
                            let rhs = self.parse_factor()?;
                            val *= rhs;
                        }
                        Some('/') => {
                            self.chars.next();
                            let rhs = self.parse_factor()?;
                            if rhs.abs() < f64::EPSILON {
                                return None;
                            }
                            val /= rhs;
                        }
                        Some('%') => {
                            self.chars.next();
                            let rhs = self.parse_factor()?;
                            if rhs.abs() < f64::EPSILON {
                                return None;
                            }
                            val %= rhs;
                        }
                        _ => break,
                    }
                }
                Some(val)
            }

            fn parse_factor(&mut self) -> Option<f64> {
                let base = self.parse_unary()?;
                self.skip_whitespace();
                if let Some('^') = self.chars.peek() {
                    self.chars.next();
                    let exp = self.parse_factor()?;
                    Some(base.powf(exp))
                } else {
                    Some(base)
                }
            }

            fn parse_unary(&mut self) -> Option<f64> {
                self.skip_whitespace();
                match self.chars.peek() {
                    Some('+') => {
                        self.chars.next();
                        self.parse_unary()
                    }
                    Some('-') => {
                        self.chars.next();
                        Some(-self.parse_unary()?)
                    }
                    _ => self.parse_primary(),
                }
            }

            fn parse_primary(&mut self) -> Option<f64> {
                self.skip_whitespace();
                match self.chars.peek() {
                    Some('(') => {
                        self.chars.next();
                        let val = self.parse_expr()?;
                        self.skip_whitespace();
                        if self.chars.next() == Some(')') {
                            Some(val)
                        } else {
                            None
                        }
                    }
                    Some(c) if c.is_ascii_digit() || *c == '.' => {
                        let mut num_str = String::new();
                        let mut seen_dot = false;
                        while let Some(&ch) = self.chars.peek() {
                            if ch.is_ascii_digit() {
                                num_str.push(ch);
                                self.chars.next();
                            } else if ch == '.' && !seen_dot {
                                seen_dot = true;
                                num_str.push(ch);
                                self.chars.next();
                            } else {
                                break;
                            }
                        }
                        num_str.parse::<f64>().ok()
                    }
                    Some(c) if c.is_alphabetic() => {
                        let mut ident = String::new();
                        while let Some(&ch) = self.chars.peek() {
                            if ch.is_alphabetic() {
                                ident.push(ch);
                                self.chars.next();
                            } else {
                                break;
                            }
                        }
                        let ident_lower = ident.to_lowercase();
                        match ident_lower.as_str() {
                            "pi" => Some(std::f64::consts::PI),
                            "e" => Some(std::f64::consts::E),
                            "sqrt" => {
                                self.skip_whitespace();
                                if self.chars.next() == Some('(') {
                                    let arg = self.parse_expr()?;
                                    self.skip_whitespace();
                                    if self.chars.next() == Some(')') && arg >= 0.0 {
                                        Some(arg.sqrt())
                                    } else {
                                        None
                                    }
                                } else {
                                    None
                                }
                            }
                            "abs" => {
                                self.skip_whitespace();
                                if self.chars.next() == Some('(') {
                                    let arg = self.parse_expr()?;
                                    self.skip_whitespace();
                                    if self.chars.next() == Some(')') {
                                        Some(arg.abs())
                                    } else {
                                        None
                                    }
                                } else {
                                    None
                                }
                            }
                            "sin" => {
                                self.skip_whitespace();
                                if self.chars.next() == Some('(') {
                                    let arg = self.parse_expr()?;
                                    self.skip_whitespace();
                                    if self.chars.next() == Some(')') {
                                        Some(arg.sin())
                                    } else {
                                        None
                                    }
                                } else {
                                    None
                                }
                            }
                            "cos" => {
                                self.skip_whitespace();
                                if self.chars.next() == Some('(') {
                                    let arg = self.parse_expr()?;
                                    self.skip_whitespace();
                                    if self.chars.next() == Some(')') {
                                        Some(arg.cos())
                                    } else {
                                        None
                                    }
                                } else {
                                    None
                                }
                            }
                            _ => None,
                        }
                    }
                    _ => None,
                }
            }
        }

        let mut parser = Parser::new(clean);
        let result = parser.parse_expr()?;
        parser.skip_whitespace();
        if parser.chars.next().is_none() {
            Some(result)
        } else {
            None
        }
    }

    pub fn fuzzy_match(pattern: &str, text: &str) -> bool {
        let p_lower = pattern.to_lowercase();
        let t_lower = text.to_lowercase();
        if t_lower.contains(&p_lower) {
            return true;
        }
        let mut p_iter = p_lower.chars();
        let mut target_char = match p_iter.next() {
            Some(c) => c,
            None => return true,
        };
        for ch in t_lower.chars() {
            if ch == target_char {
                match p_iter.next() {
                    Some(next_c) => target_char = next_c,
                    None => return true,
                }
            }
        }
        false
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

        // 2. Coincidencia difusa y por relevancia con aplicaciones
        let mut app_matches: Vec<(u32, SearchResultItem)> = Vec::new();

        // Aplicaciones Quartz integradas (prioridad alta de paridad de sistema)
        let built_in_apps = [
            ("Ajustes del Sistema", "quartz-settings", "Configuración y preferencias de QuartzDE", "preferences-system"),
            ("Finder", "quartz-finder", "Gestor de archivos nativo", "system-file-manager"),
            ("Terminal", "x-terminal-emulator", "Emulador de terminal del sistema", "utilities-terminal"),
            ("Monitor de Actividad", "plasma-systemmonitor", "Rendimiento y procesos", "utilities-system-monitor"),
        ];

        for (name, exec, desc, icon) in built_in_apps {
            let n_lower = name.to_lowercase();
            if n_lower.starts_with(&query_lower) {
                app_matches.push((120, SearchResultItem {
                    title: name.to_string(),
                    subtitle: desc.to_string(),
                    category: SearchResultCategory::Application,
                    icon_name: icon.to_string(),
                    action_payload: exec.to_string(),
                }));
            } else if n_lower.contains(&query_lower) || Self::fuzzy_match(&query_lower, name) {
                app_matches.push((90, SearchResultItem {
                    title: name.to_string(),
                    subtitle: desc.to_string(),
                    category: SearchResultCategory::Application,
                    icon_name: icon.to_string(),
                    action_payload: exec.to_string(),
                }));
            }
        }

        let apps = Self::get_apps();
        for app in apps {
            let n_lower = app.name.to_lowercase();
            let score = if n_lower.starts_with(&query_lower) {
                100
            } else if n_lower.contains(&query_lower) {
                80
            } else if app.generic_name.to_lowercase().contains(&query_lower) {
                60
            } else if Self::fuzzy_match(&query_lower, &app.name) {
                40
            } else if Self::fuzzy_match(&query_lower, &app.comment) {
                20
            } else {
                0
            };

            if score > 0 {
                // Evitar duplicados con apps built-in
                if !app_matches.iter().any(|(_, r)| r.title.eq_ignore_ascii_case(&app.name)) {
                    let subtitle = if !app.comment.is_empty() {
                        app.comment.clone()
                    } else if !app.generic_name.is_empty() {
                        app.generic_name.clone()
                    } else {
                        app.exec.clone()
                    };
                    app_matches.push((score, SearchResultItem {
                        title: app.name.clone(),
                        subtitle,
                        category: SearchResultCategory::Application,
                        icon_name: app.icon.clone(),
                        action_payload: app.exec.clone(),
                    }));
                }
            }
        }

        // Ordenar por score descendente
        app_matches.sort_by(|a, b| b.0.cmp(&a.0));
        for (_, item) in app_matches {
            results.push(item);
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

    async fn list_apps(&self) -> zbus::fdo::Result<Vec<(String, String, String, String)>> {
        let apps = SpotlightEngine::get_apps();
        let serializable = apps
            .iter()
            .map(|a| (a.name.clone(), a.generic_name.clone(), a.icon.clone(), a.exec.clone()))
            .collect();
        Ok(serializable)
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
        assert_eq!(SpotlightEngine::evaluate_math("2 + 3 * 4"), Some(14.0));
        assert_eq!(SpotlightEngine::evaluate_math("(10 - 2) * 5"), Some(40.0));
        assert_eq!(SpotlightEngine::evaluate_math("2 ^ 3"), Some(8.0));
        assert_eq!(SpotlightEngine::evaluate_math("sqrt(16)"), Some(4.0));
        assert_eq!(SpotlightEngine::evaluate_math("invalid"), None);
    }

    #[test]
    fn test_fuzzy_matching() {
        assert!(SpotlightEngine::fuzzy_match("term", "Terminal"));
        assert!(SpotlightEngine::fuzzy_match("fnd", "Finder"));
        assert!(SpotlightEngine::fuzzy_match("calc", "accessories-calculator"));
        assert!(!SpotlightEngine::fuzzy_match("xyz", "Terminal"));
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
