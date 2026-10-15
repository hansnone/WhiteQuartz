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

use std::env;
use std::fs;
use std::io::{self, Write};
use std::path::{Path, PathBuf};

pub fn get_config_path() -> PathBuf {
    if let Ok(config_home) = env::var("XDG_CONFIG_HOME") {
        if !config_home.trim().is_empty() {
            return PathBuf::from(config_home).join("quartz").join("config.json");
        }
    }
    if let Ok(home) = env::var("HOME") {
        return PathBuf::from(home).join(".config").join("quartz").join("config.json");
    }
    PathBuf::from("/tmp/quartz/config.json")
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct QuartzDesktopConfig {
    pub appearance: AppearanceMode,
    pub accent_color: AccentColor,
    pub dock: DockConfig,
    pub stage_manager: StageManagerConfig,
    pub theme_name: String,
    pub icon_theme: String,
}

impl Default for QuartzDesktopConfig {
    fn default() -> Self {
        Self {
            appearance: AppearanceMode::default(),
            accent_color: AccentColor::default(),
            dock: DockConfig::default(),
            stage_manager: StageManagerConfig::default(),
            theme_name: "Aqua".to_string(),
            icon_theme: "Colloid".to_string(),
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

    pub fn load_from_xdg() -> Self {
        let path = get_config_path();
        if path.exists() {
            if let Ok(content) = fs::read_to_string(&path) {
                if let Ok(cfg) = Self::from_json(&content) {
                    return cfg;
                }
            }
        }
        let default_cfg = Self::default();
        let _ = default_cfg.save_to_xdg();
        default_cfg
    }

    pub fn save_to_xdg(&self) -> io::Result<()> {
        let path = get_config_path();
        self.save_to_path(&path)
    }

    pub fn save_to_path(&self, path: &Path) -> io::Result<()> {
        if let Some(parent) = path.parent() {
            fs::create_dir_all(parent)?;
        }
        let json = self.to_json().map_err(|e| io::Error::new(io::ErrorKind::Other, e))?;
        let tmp_path = path.with_extension("tmp");
        {
            let mut file = fs::File::create(&tmp_path)?;
            file.write_all(json.as_bytes())?;
            file.sync_all()?;
        }
        fs::rename(tmp_path, path)?;
        Ok(())
    }

    pub fn load_from_path(path: &Path) -> io::Result<Self> {
        let content = fs::read_to_string(path)?;
        Self::from_json(&content).map_err(|e| io::Error::new(io::ErrorKind::InvalidData, e))
    }
}
