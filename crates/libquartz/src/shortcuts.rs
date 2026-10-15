use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct DesktopShortcuts {
    // Ventanas y Aplicaciones
    pub close_window: String,      // "Ctrl+W"
    pub close_all_windows: String,  // "Ctrl+Alt+W"
    pub quit_app: String,          // "Ctrl+Q"
    pub minimize_window: String,   // "Ctrl+M"
    pub hide_app: String,          // "Ctrl+H"

    // Búsqueda y Previsualización
    pub spotlight_search: String,  // "Ctrl+Space"
    pub quick_look: String,        // "Space"

    // Modos de Finder (Conmutación de vistas)
    pub finder_view_icons: String,   // "Ctrl+1"
    pub finder_view_list: String,    // "Ctrl+2"
    pub finder_view_columns: String, // "Ctrl+3"
    pub finder_view_gallery: String, // "Ctrl+4"

    // Gestión Espacial (Teclas Windows / Super)
    pub mission_control: String,     // "Super+Up"
    pub app_expose: String,          // "Super+Down"
    pub space_left: String,          // "Super+Left"
    pub space_right: String,         // "Super+Right"
}

impl Default for DesktopShortcuts {
    fn default() -> Self {
        Self {
            close_window: "Ctrl+W".to_string(),
            close_all_windows: "Ctrl+Alt+W".to_string(),
            quit_app: "Ctrl+Q".to_string(),
            minimize_window: "Ctrl+M".to_string(),
            hide_app: "Ctrl+H".to_string(),
            spotlight_search: "Ctrl+Space".to_string(),
            quick_look: "Space".to_string(),
            finder_view_icons: "Ctrl+1".to_string(),
            finder_view_list: "Ctrl+2".to_string(),
            finder_view_columns: "Ctrl+3".to_string(),
            finder_view_gallery: "Ctrl+4".to_string(),
            mission_control: "Super+Up".to_string(),
            app_expose: "Super+Down".to_string(),
            space_left: "Super+Left".to_string(),
            space_right: "Super+Right".to_string(),
        }
    }
}

impl DesktopShortcuts {
    pub fn to_json(&self) -> Result<String, serde_json::Error> {
        serde_json::to_string_pretty(self)
    }

    pub fn from_json(json_str: &str) -> Result<Self, serde_json::Error> {
        serde_json::from_str(json_str)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_windows_keyboard_shortcuts() {
        let shortcuts = DesktopShortcuts::default();
        // Verificar que ningún atajo usa Cmd ni Option
        let json = shortcuts.to_json().unwrap();
        assert!(!json.contains("Cmd"));
        assert!(!json.contains("Command"));
        assert!(!json.contains("Option"));

        assert_eq!(shortcuts.close_window, "Ctrl+W");
        assert_eq!(shortcuts.quit_app, "Ctrl+Q");
        assert_eq!(shortcuts.spotlight_search, "Ctrl+Space");
        assert_eq!(shortcuts.mission_control, "Super+Up");
        assert_eq!(shortcuts.finder_view_columns, "Ctrl+3");

        let deserialized = DesktopShortcuts::from_json(&json).unwrap();
        assert_eq!(deserialized.close_window, "Ctrl+W");
    }
}
