#include "quartzdecoration.h"

namespace Quartz::Compositor {

QuartzDecoration::QuartzDecoration(QObject *parent)
    : QObject(parent)
{
    m_zoomHoverTimer.setSingleShot(true);
    m_zoomHoverTimer.setInterval(500); // 500 ms de hover para desplegar Window Tiling
    connect(&m_zoomHoverTimer, &QTimer::timeout, this, &QuartzDecoration::onZoomHoverTimerTimeout);
}

void QuartzDecoration::setUnifiedToolbar(bool unified)
{
    if (m_unifiedToolbar != unified) {
        m_unifiedToolbar = unified;
        Q_EMIT titleBarHeightChanged(titleBarHeight());
    }
}

int QuartzDecoration::titleBarHeight() const
{
    return m_unifiedToolbar
        ? StyleMetrics::UnifiedToolbarTitleHeight
        : StyleMetrics::StandardTitleBarHeight;
}

QRect QuartzDecoration::closeButtonRect() const
{
    int y = (titleBarHeight() - StyleMetrics::TrafficLightButtonSize) / 2;
    return QRect(StyleMetrics::TrafficLightLeftMargin, y, StyleMetrics::TrafficLightButtonSize, StyleMetrics::TrafficLightButtonSize);
}

QRect QuartzDecoration::minimizeButtonRect() const
{
    int y = (titleBarHeight() - StyleMetrics::TrafficLightButtonSize) / 2;
    int step = StyleMetrics::TrafficLightButtonSize + StyleMetrics::TrafficLightSpacing;
    return QRect(StyleMetrics::TrafficLightLeftMargin + step, y, StyleMetrics::TrafficLightButtonSize, StyleMetrics::TrafficLightButtonSize);
}

QRect QuartzDecoration::maximizeButtonRect() const
{
    int y = (titleBarHeight() - StyleMetrics::TrafficLightButtonSize) / 2;
    int step = StyleMetrics::TrafficLightButtonSize + StyleMetrics::TrafficLightSpacing;
    return QRect(StyleMetrics::TrafficLightLeftMargin + 2 * step, y, StyleMetrics::TrafficLightButtonSize, StyleMetrics::TrafficLightButtonSize);
}

void QuartzDecoration::setDocumentHasUnsavedChanges(bool unsaved)
{
    if (m_unsavedChanges != unsaved) {
        m_unsavedChanges = unsaved;
        Q_EMIT unsavedChangesChanged(m_unsavedChanges);
    }
}

void QuartzDecoration::setHoverState(TrafficLightHoverState state)
{
    if (m_hoverState != state) {
        m_hoverState = state;
        Q_EMIT hoverStateChanged(m_hoverState);
    }
}

void QuartzDecoration::onZoomHoverEntered(const QPoint &globalPos)
{
    m_lastZoomPos = globalPos;
    setHoverState(TrafficLightHoverState::ZoomHovered);
    m_zoomHoverTimer.start();
}

void QuartzDecoration::onZoomHoverLeft()
{
    m_zoomHoverTimer.stop();
    m_tilingMenuVisible = false;
    if (m_hoverState == TrafficLightHoverState::ZoomHovered) {
        setHoverState(TrafficLightHoverState::None);
    }
}

void QuartzDecoration::onZoomHoverTimerTimeout()
{
    m_tilingMenuVisible = true;
    Q_EMIT tilingMenuRequested(m_lastZoomPos);
}

void QuartzDecoration::applyTileMode(WindowTileMode mode)
{
    m_tilingMenuVisible = false;
    Q_EMIT tileModeApplied(mode);
}

} // namespace Quartz::Compositor
