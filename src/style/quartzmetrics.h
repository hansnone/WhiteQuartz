#pragma once

namespace Quartz::StyleMetrics {
    // Dimensiones de Paneles y Barras del Sistema
    inline constexpr int MenuBarHeight             = 24;
    inline constexpr int DockBaseIconSize          = 48;
    inline constexpr int DockMaxMagnifiedSize      = 96;
    inline constexpr int DockCornerRadius          = 18;
    inline constexpr int DockBottomMargin          = 6;

    // Ventanas y Decoraciones
    inline constexpr int WindowCornerRadius        = 11; // Superelipse n=4
    inline constexpr int StandardTitleBarHeight    = 28;
    inline constexpr int UnifiedToolbarTitleHeight = 52;
    inline constexpr int TrafficLightButtonSize    = 12;
    inline constexpr int TrafficLightSpacing       = 8;
    inline constexpr int TrafficLightLeftMargin    = 14;

    // Controles Nativos (QuartzStyle / QQC2)
    inline constexpr int PushButtonHeight          = 22;
    inline constexpr int PushButtonRadius          = 5;
    inline constexpr int SegmentedControlRadius    = 6;
    inline constexpr int InsetGroupCardRadius      = 10;
    inline constexpr int SwitchWidth               = 38;
    inline constexpr int SwitchHeight              = 22;
    inline constexpr int SidebarBlurMaterialAlpha  = 175; // Translucidez Vibrancy
}
