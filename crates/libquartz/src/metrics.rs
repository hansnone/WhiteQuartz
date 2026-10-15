//! Métricas visuales y constantes geométricas estrictas de QuartzDE
//! Inspiradas en Apple Human Interface Guidelines (macOS Sonoma / Sequoia).

pub const MENUBAR_HEIGHT: u32 = 24;
pub const DOCK_BASE_ICON_SIZE: u32 = 48;
pub const DOCK_MAX_MAGNIFIED_SIZE: u32 = 96;
pub const DOCK_CORNER_RADIUS: u32 = 18;
pub const DOCK_BOTTOM_MARGIN: u32 = 6;

pub const WINDOW_CORNER_RADIUS: u32 = 11;
pub const STANDARD_TITLEBAR_HEIGHT: u32 = 28;
pub const UNIFIED_TOOLBAR_TITLE_HEIGHT: u32 = 52;
pub const TRAFFIC_LIGHT_BUTTON_SIZE: u32 = 12;
pub const TRAFFIC_LIGHT_SPACING: u32 = 8;
pub const TRAFFIC_LIGHT_LEFT_MARGIN: u32 = 14;

pub const PUSH_BUTTON_HEIGHT: u32 = 22;
pub const PUSH_BUTTON_RADIUS: u32 = 5;
pub const SEGMENTED_CONTROL_RADIUS: u32 = 6;
pub const INSET_GROUP_CARD_RADIUS: u32 = 10;
pub const SWITCH_WIDTH: u32 = 38;
pub const SWITCH_HEIGHT: u32 = 22;
pub const SIDEBAR_BLUR_ALPHA: u8 = 175;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct GeometryRect {
    pub x: i32,
    pub y: i32,
    pub width: u32,
    pub height: u32,
}

impl GeometryRect {
    pub const fn new(x: i32, y: i32, width: u32, height: u32) -> Self {
        Self { x, y, width, height }
    }
}
