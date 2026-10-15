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
    }
}
