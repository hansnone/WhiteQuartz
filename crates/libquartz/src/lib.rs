pub mod config;
pub mod dbus;
pub mod metrics;
pub mod shortcuts;

pub use config::QuartzDesktopConfig;
pub use metrics::*;
pub use shortcuts::DesktopShortcuts;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_metrics_dimensions() {
        assert_eq!(MENUBAR_HEIGHT, 24);
        assert_eq!(DOCK_BASE_ICON_SIZE, 48);
        assert_eq!(DOCK_MAX_MAGNIFIED_SIZE, 96);
        assert_eq!(WINDOW_CORNER_RADIUS, 11);
        assert_eq!(UNIFIED_TOOLBAR_TITLE_HEIGHT, 52);
    }

    #[test]
    fn test_desktop_config_serialization() {
        let config = QuartzDesktopConfig::default();
        let json_str = config.to_json().expect("Failed to serialize config");
        assert!(json_str.contains("magnification"));
        assert!(json_str.contains("Auto"));

        let deserialized = QuartzDesktopConfig::from_json(&json_str).expect("Failed to deserialize");
        assert_eq!(deserialized.dock.base_size, 48);
        assert!(deserialized.dock.magnification);
        assert_eq!(deserialized.theme_name, "Aqua");
        assert_eq!(deserialized.icon_theme, "Colloid");
    }

    #[test]
    fn test_desktop_config_atomic_file_persistence() {
        let temp_dir = std::env::temp_dir();
        let test_file = temp_dir.join(format!("quartz_test_config_{}.json", std::process::id()));

        let mut config = QuartzDesktopConfig::default();
        config.theme_name = "Orchis-Dark".to_string();
        config.stage_manager.enabled = true;

        assert!(config.save_to_path(&test_file).is_ok());
        assert!(test_file.exists());

        let loaded = QuartzDesktopConfig::load_from_path(&test_file).expect("Failed to load config");
        assert_eq!(loaded.theme_name, "Orchis-Dark");
        assert!(loaded.stage_manager.enabled);

        let _ = std::fs::remove_file(&test_file);
    }

    #[test]
    fn test_desktop_config_backward_compatibility() {
        // JSON heredado de Fase 3 (sin el campo nuevo "wallpaper" y con campos omitidos)
        let legacy_json = r#"{
            "appearance": "Dark",
            "accent_color": "Blue",
            "dock": {
                "base_size": 52,
                "magnification": false,
                "max_magnified_size": 90
            },
            "theme_name": "Orchis-Dark",
            "icon_theme": "Colloid"
        }"#;

        let deserialized = QuartzDesktopConfig::from_json(legacy_json).expect("Debe deserializar JSON heredado con serde(default)");
        assert_eq!(deserialized.appearance, config::AppearanceMode::Dark);
        assert_eq!(deserialized.accent_color, config::AccentColor::Blue);
        assert_eq!(deserialized.dock.base_size, 52);
        assert!(!deserialized.dock.magnification);
        // Debe rellenar con defaults los campos que faltaban
        assert!(deserialized.dock.bouncing_animation);
        assert_eq!(deserialized.wallpaper, "/usr/share/backgrounds/quartz.png");
    }

    #[test]
    fn test_desktop_config_bad_backup() {
        let temp_dir = std::env::temp_dir();
        let test_dir = temp_dir.join(format!("quartz_bad_test_{}", std::process::id()));
        let _ = std::fs::create_dir_all(&test_dir);
        let test_file = test_dir.join("config.json");
        let bad_file = test_dir.join("config.json.bad");

        // Escribir JSON inválido / corrupto
        let _ = std::fs::write(&test_file, b"{ invalid json content ... ");

        // Simular lógica de load_from_xdg
        if test_file.exists() {
            if let Ok(content) = std::fs::read_to_string(&test_file) {
                if QuartzDesktopConfig::from_json(&content).is_err() {
                    let _ = std::fs::rename(&test_file, &bad_file);
                }
            }
        }

        assert!(!test_file.exists());
        assert!(bad_file.exists());

        let _ = std::fs::remove_file(&bad_file);
        let _ = std::fs::remove_dir(&test_dir);
    }
}
