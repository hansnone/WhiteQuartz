//! Configuración reactiva y modelos de estado persistente de QuartzDE.

use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum AppearanceMode {
    Light,
    Dark,
    Auto,
}

impl Default for AppearanceMode {
    fn default() -> Self {
        Self::Auto
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum AccentColor {
    Multicolor,
    Blue,
    Purple,
    Pink,
    Red,
    Orange,
    Yellow,
    Green,
    Graphite,
}

impl Default for AccentColor {
    fn default() -> Self {
        Self::Multicolor
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DockConfig {
    pub base_size: u32,
    pub magnification: bool,
    pub max_magnified_size: u32,
    pub autohide: bool,
    pub bouncing_animation: bool,
    pub show_recent_apps: bool,
}

impl Default for DockConfig {
    fn default() -> Self {
        Self {
            base_size: 48,
            magnification: true,
            max_magnified_size: 96,
            autohide: false,
            bouncing_animation: true,
            show_recent_apps: true,
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StageManagerConfig {
    pub enabled: bool,
    pub perspective_angle_y: f64,
    pub strip_scale_factor: f64,
    pub hide_desktop_icons: bool,
}

impl Default for StageManagerConfig {
    fn default() -> Self {
        Self {
            enabled: false,
            perspective_angle_y: 14.0,
            strip_scale_factor: 0.18,
            hide_desktop_icons: true,
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct QuartzDesktopConfig {
    pub appearance: AppearanceMode,
    pub accent_color: AccentColor,
    pub dock: DockConfig,
    pub stage_manager: StageManagerConfig,
}

impl Default for QuartzDesktopConfig {
    fn default() -> Self {
        Self {
            appearance: AppearanceMode::default(),
            accent_color: AccentColor::default(),
            dock: DockConfig::default(),
            stage_manager: StageManagerConfig::default(),
        }
    }
}

impl QuartzDesktopConfig {
    pub fn to_json(&self) -> Result<String, serde_json::Error> {
        serde_json::to_string_pretty(self)
    }

    pub fn from_json(json_str: &str) -> Result<Self, serde_json::Error> {
        serde_json::from_str(json_str)
    }
}
