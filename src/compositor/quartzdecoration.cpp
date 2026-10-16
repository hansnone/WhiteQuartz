#include "quartzdecoration.h"
#include <QPainter>
#include "../style/quartzstyle.h"
#include "stagemanagereffect.h"

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

void QuartzDecoration::paintTrafficLights(QPainter *painter, const QRect &titleBarRect, TrafficLightHoverState hoverState, bool unsaved)
{
    if (!painter) return;

    Quartz::Style::QuartzStyle style;
    bool isHovered = (hoverState != TrafficLightHoverState::None);

    int y = titleBarRect.top() + (titleBarRect.height() - StyleMetrics::TrafficLightButtonSize) / 2;
    int step = StyleMetrics::TrafficLightButtonSize + StyleMetrics::TrafficLightSpacing;

    QRect closeR(titleBarRect.left() + StyleMetrics::TrafficLightLeftMargin, y, StyleMetrics::TrafficLightButtonSize, StyleMetrics::TrafficLightButtonSize);
    QRect minR(titleBarRect.left() + StyleMetrics::TrafficLightLeftMargin + step, y, StyleMetrics::TrafficLightButtonSize, StyleMetrics::TrafficLightButtonSize);
    QRect maxR(titleBarRect.left() + StyleMetrics::TrafficLightLeftMargin + 2 * step, y, StyleMetrics::TrafficLightButtonSize, StyleMetrics::TrafficLightButtonSize);

    char closeSym = isHovered ? 'x' : '\0';
    bool closeDot = (!isHovered && unsaved);
    char minSym = isHovered ? '-' : '\0';
    char maxSym = isHovered ? '+' : '\0';

    style.drawTrafficLightButton(painter, closeR, QColor(255, 95, 87), closeSym, closeDot);
    style.drawTrafficLightButton(painter, minR, QColor(254, 188, 46), minSym, false);
    style.drawTrafficLightButton(painter, maxR, QColor(40, 201, 64), maxSym, false);
}

void QuartzDecoration::drawTitleBar(QPainter *painter, const QRect &titleBarRect, bool active, const QString &caption, const QColor &textColor)
{
    if (!painter) return;

    painter->save();

    // Fondo sutil del titlebar (estilo macOS Sonoma)
    QColor bgColor = active ? QColor(45, 45, 48, 240) : QColor(35, 35, 37, 220);
    painter->fillRect(titleBarRect, bgColor);

    // Borde inferior separador
    painter->setPen(QColor(255, 255, 255, 20));
    painter->drawLine(titleBarRect.bottomLeft(), titleBarRect.bottomRight());

    // Título de la ventana centrado
    if (!caption.isEmpty()) {
        painter->setPen(active ? textColor : QColor(160, 160, 160));
        QFont font = painter->font();
        font.setPointSize(11);
        font.setWeight(QFont::Medium);
        painter->setFont(font);

        painter->drawText(titleBarRect, Qt::AlignCenter, caption);
    }

    painter->restore();
}

QImage QuartzDecoration::generateShadowImage(int width, int height, bool active)
{
    DropShadowParams shadow = StageManagerEffect::shadowForWindow(active);
    int pad = static_cast<int>(shadow.blurRadius + shadow.yOffset);
    int totalW = qMax(width + pad * 2, 64);
    int totalH = qMax(height + pad * 2, 64);

    QImage img(totalW, totalH, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);

    QRectF inner(pad, pad + shadow.yOffset, width, height);
    QColor shadowCol(0, 0, 0, static_cast<int>(255 * shadow.opacity));

    p.setPen(Qt::NoPen);
    p.setBrush(shadowCol);
    p.drawRoundedRect(inner, StyleMetrics::WindowCornerRadius, StyleMetrics::WindowCornerRadius);

    return img;
}

} // namespace Quartz::Compositor

