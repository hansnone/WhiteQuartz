use serde::{Deserialize, Serialize};
use zbus::proxy;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub struct ItemGeometry {
    pub x: i32,
    pub y: i32,
    pub width: i32,
    pub height: i32,
}

impl ItemGeometry {
    pub fn new(x: i32, y: i32, width: i32, height: i32) -> Self {
        Self { x, y, width, height }
    }
}

#[proxy(
    default_service = "org.quartz.QuickLook",
    default_path = "/org/quartz/QuickLook",
    interface = "org.quartz.QuickLook"
)]
pub trait QuickLook {
    /// Alterna la visualización previa rápida de un archivo
    fn toggle_preview(&self, file_uri: &str, geometry: (i32, i32, i32, i32)) -> zbus::Result<()>;

    /// Actualiza el fichero activo cuando se navega con flechas
    fn update_selection(&self, file_uri: &str) -> zbus::Result<()>;

    /// Cierra la ventana de previsualización
    fn close_preview(&self) -> zbus::Result<()>;
}
