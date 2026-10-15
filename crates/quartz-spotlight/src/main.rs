use clap::Parser;
use tracing::info;
use zbus::{connection, interface};

#[derive(Parser, Debug)]
#[command(author, version, about = "QuartzDE Spotlight Search Service")]
struct Args {
    #[arg(short, long, help = "Ejecutar consulta única desde línea de comandos")]
    query: Option<String>,
}

#[derive(Debug, Clone, PartialEq)]
pub enum SearchResultCategory {
    Calculator,
    Application,
    File,
    Setting,
}

#[derive(Debug, Clone, PartialEq)]
pub struct SearchResultItem {
    pub title: String,
    pub subtitle: String,
    pub category: SearchResultCategory,
    pub action_payload: String,
}

pub struct SpotlightEngine;

impl SpotlightEngine {
    pub fn evaluate_math(input: &str) -> Option<f64> {
        let clean = input.trim().replace(' ', "");
        if clean.is_empty() || !clean.chars().all(|c| c.is_ascii_digit() || "+-*/.".contains(c)) {
            return None;
        }

        // Evaluador aritmético seguro y sencillo para sumas y multiplicaciones
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
                action_payload: format!("{val}"),
            });
        }

        // 2. Coincidencia con aplicaciones del sistema Quartz
        let known_apps = [
            ("Ajustes del Sistema", "quartz-settings", "Configuración y preferencias de QuartzDE"),
            ("Finder", "quartz-finder", "Gestor de archivos nativo"),
            ("Terminal", "x-terminal-emulator", "Emulador de terminal del sistema"),
            ("Monitor de Actividad", "plasma-systemmonitor", "Rendimiento y procesos"),
        ];

        for (name, exec, desc) in known_apps {
            if name.to_lowercase().contains(&trimmed.to_lowercase()) {
                results.push(SearchResultItem {
                    title: name.to_string(),
                    subtitle: desc.to_string(),
                    category: SearchResultCategory::Application,
                    action_payload: exec.to_string(),
                });
            }
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

    async fn search(&self, text: String) -> zbus::fdo::Result<Vec<(String, String, String)>> {
        let items = SpotlightEngine::query(&text);
        let serializable = items
            .into_iter()
            .map(|i| (i.title, i.subtitle, i.action_payload))
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
        assert_eq!(SpotlightEngine::evaluate_math("invalid"), None);
    }

    #[test]
    fn test_app_query() {
        let results = SpotlightEngine::query("Ajustes");
        assert!(!results.is_empty());
        assert_eq!(results[0].title, "Ajustes del Sistema");
    }
}
