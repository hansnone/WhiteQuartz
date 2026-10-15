use std::fs;
use std::path::Path;
use std::sync::Arc;
use tokio::sync::Mutex;
use tracing::info;
use zbus::{connection, interface};

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum PreviewType {
    Image,
    Text,
    Pdf,
    Audio,
    Video,
    Generic,
}

#[derive(Debug, Clone)]
pub struct FilePreviewInfo {
    pub uri: String,
    pub path: String,
    pub file_name: String,
    pub preview_type: PreviewType,
    pub size_bytes: u64,
    pub summary: String,
}

pub struct PreviewInspector;

impl PreviewInspector {
    pub fn inspect(uri: &str) -> FilePreviewInfo {
        let path_str = uri.trim_start_matches("file://");
        let path = Path::new(path_str);
        let file_name = path
            .file_name()
            .map(|s| s.to_string_lossy().to_string())
            .unwrap_or_else(|| "Desconocido".to_string());

        let size_bytes = fs::metadata(path).map(|m| m.len()).unwrap_or(0);
        let ext = path
            .extension()
            .and_then(|e| e.to_str())
            .unwrap_or("")
            .to_lowercase();

        let (preview_type, summary) = match ext.as_str() {
            "png" | "jpg" | "jpeg" | "webp" | "svg" | "gif" => {
                (PreviewType::Image, format!("Imagen ({}) - {} bytes", ext.to_uppercase(), size_bytes))
            }
            "txt" | "rs" | "cpp" | "h" | "hpp" | "qml" | "py" | "sh" | "json" | "toml" | "md" => {
                let lines = fs::read_to_string(path)
                    .map(|s| s.lines().count())
                    .unwrap_or(0);
                (PreviewType::Text, format!("Código/Texto ({} líneas) - {} bytes", lines, size_bytes))
            }
            "pdf" => (PreviewType::Pdf, format!("Documento PDF - {} bytes", size_bytes)),
            "mp3" | "flac" | "wav" | "ogg" => (PreviewType::Audio, format!("Audio ({}) - {} bytes", ext.to_uppercase(), size_bytes)),
            "mp4" | "mkv" | "webm" | "mov" => (PreviewType::Video, format!("Vídeo ({}) - {} bytes", ext.to_uppercase(), size_bytes)),
            _ => (PreviewType::Generic, format!("Archivo binario - {} bytes", size_bytes)),
        };

        FilePreviewInfo {
            uri: uri.to_string(),
            path: path_str.to_string(),
            file_name,
            preview_type,
            size_bytes,
            summary,
        }
    }
}

pub struct QuickLookServer {
    current_preview: Arc<Mutex<Option<FilePreviewInfo>>>,
}

impl QuickLookServer {
    pub fn new() -> Self {
        Self {
            current_preview: Arc::new(Mutex::new(None)),
        }
    }
}

#[interface(name = "org.quartz.QuickLook")]
impl QuickLookServer {
    async fn toggle_preview(&mut self, file_uri: String, _geometry: (i32, i32, i32, i32)) -> zbus::fdo::Result<()> {
        let mut lock = self.current_preview.lock().await;
        if let Some(current) = &*lock {
            if current.uri == file_uri {
                info!("Cerrando Quick Look para: {}", file_uri);
                *lock = None;
                return Ok(());
            }
        }

        let info = PreviewInspector::inspect(&file_uri);
        info!("Abriendo Quick Look para: {} [{}]", info.file_name, info.summary);
        *lock = Some(info);
        Ok(())
    }

    async fn update_selection(&mut self, file_uri: String) -> zbus::fdo::Result<()> {
        let mut lock = self.current_preview.lock().await;
        if lock.is_some() {
            let info = PreviewInspector::inspect(&file_uri);
            info!("Actualizando Quick Look a: {} [{}]", info.file_name, info.summary);
            *lock = Some(info);
        }
        Ok(())
    }

    async fn close_preview(&mut self) -> zbus::fdo::Result<()> {
        let mut lock = self.current_preview.lock().await;
        info!("Cierre explícito de Quick Look solicitado.");
        *lock = None;
        Ok(())
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    tracing_subscriber::fmt::init();
    info!("Iniciando demonio nativo en Rust quartz-quicklookd...");

    let server = QuickLookServer::new();
    let _conn = connection::Builder::session()?
        .name("org.quartz.QuickLook")?
        .serve_at("/org/quartz/QuickLook", server)?
        .build()
        .await?;

    info!("quartz-quicklookd registrado en D-Bus (org.quartz.QuickLook) con éxito.");

    // Mantener el demonio vivo
    std::future::pending::<()>().await;
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::Path;

    #[test]
    fn test_inspect_text_file() {
        let manifest_dir = env!("CARGO_MANIFEST_DIR");
        let toml_path = Path::new(manifest_dir).join("Cargo.toml");
        let uri = format!("file://{}", toml_path.to_string_lossy());

        let info = PreviewInspector::inspect(&uri);
        assert_eq!(info.preview_type, PreviewType::Text);
        assert_eq!(info.file_name, "Cargo.toml");
        assert!(info.size_bytes > 0, "El tamaño de Cargo.toml debe ser mayor que 0");
    }

    #[test]
    fn test_inspect_non_existent_file() {
        let info = PreviewInspector::inspect("file:///tmp/non_existent_file_test_quartz.xyz");
        assert_eq!(info.preview_type, PreviewType::Generic);
        assert_eq!(info.size_bytes, 0);
    }
}
