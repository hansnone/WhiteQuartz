#include "quartzdecoration_plugin.h"
#include "stagemanagereffect.h"
#include <KPluginFactory>
#include <QHoverEvent>
#include <QMouseEvent>
#include <QPainter>

namespace Quartz::Compositor {

K_PLUGIN_CLASS_WITH_JSON(QuartzDecorationPlugin, "decoration.json")

QuartzDecorationPlugin::QuartzDecorationPlugin(QObject *parent, const QVariantList &args)
    : KDecoration3::Decoration(parent, args)
{
}

bool QuartzDecorationPlugin::init()
{
    updateBordersAndTitleBar();

    if (window()) {
        connect(window(), &KDecoration3::DecoratedWindow::activeChanged, this, [this]() {
            updateShadow();
            update();
        });
        connect(window(), &KDecoration3::DecoratedWindow::captionChanged, this, qOverload<>(&KDecoration3::Decoration::update));
        connect(window(), &KDecoration3::DecoratedWindow::maximizedChanged, this, [this]() {
            updateBordersAndTitleBar();
            update();
        });
        updateShadow();
    }

    connect(&m_core, &QuartzDecoration::tilingMenuRequested, this, [this](const QPoint &globalPos) {
        requestShowWindowMenu(QRect(globalPos, QSize(1, 1)));
    });

    return true;
}

void QuartzDecorationPlugin::updateBordersAndTitleBar()
{
    int tbHeight = m_core.titleBarHeight();
    setBorders(QMarginsF(0, tbHeight, 0, 0));
    setTitleBar(QRectF(0, 0, size().width(), tbHeight));
}

void QuartzDecorationPlugin::updateShadow()
{
    if (!window()) return;

    bool active = window()->isActive();
    DropShadowParams shadow = StageManagerEffect::shadowForWindow(active);
    int pad = static_cast<int>(shadow.blurRadius + shadow.yOffset);
    QImage shadowImg = QuartzDecoration::generateShadowImage(300, 200, active);

    m_shadow = std::make_shared<KDecoration3::DecorationShadow>();
    m_shadow->setShadow(shadowImg);
    m_shadow->setPadding(QMarginsF(pad, pad + shadow.yOffset, pad, pad));
    m_shadow->setInnerShadowRect(QRectF(pad, pad + shadow.yOffset, 300, 200));

    setShadow(m_shadow);
}

void QuartzDecorationPlugin::paint(QPainter *painter, const QRectF &/*repaintRegion*/)
{
    if (!painter) return;

    int tbHeight = m_core.titleBarHeight();
    QRect titleRect(0, 0, static_cast<int>(size().width()), tbHeight);

    bool active = window() ? window()->isActive() : true;
    QString caption = window() ? window()->caption() : QString();

    m_core.drawTitleBar(painter, titleRect, active, caption);
    m_core.paintTrafficLights(painter, titleRect, m_core.hoverState(), m_core.documentHasUnsavedChanges());
}

void QuartzDecorationPlugin::hoverMoveEvent(QHoverEvent *event)
{
    QPoint pt = event->position().toPoint();
    if (m_core.closeButtonRect().contains(pt)) {
        m_core.setHoverState(TrafficLightHoverState::CloseHovered);
        m_core.onZoomHoverLeft();
    } else if (m_core.minimizeButtonRect().contains(pt)) {
        m_core.setHoverState(TrafficLightHoverState::MinimizeHovered);
        m_core.onZoomHoverLeft();
    } else if (m_core.maximizeButtonRect().contains(pt)) {
        m_core.onZoomHoverEntered(event->globalPosition().toPoint());
    } else {
        m_core.setHoverState(TrafficLightHoverState::None);
        m_core.onZoomHoverLeft();
    }
    update();
    KDecoration3::Decoration::hoverMoveEvent(event);
}

void QuartzDecorationPlugin::hoverLeaveEvent(QHoverEvent *event)
{
    m_core.setHoverState(TrafficLightHoverState::None);
    m_core.onZoomHoverLeft();
    update();
    KDecoration3::Decoration::hoverLeaveEvent(event);
}

void QuartzDecorationPlugin::mousePressEvent(QMouseEvent *event)
{
    QPoint pt = event->position().toPoint();
    if (m_core.closeButtonRect().contains(pt) && (event->button() == Qt::LeftButton)) {
        requestClose();
        return;
    } else if (m_core.minimizeButtonRect().contains(pt) && (event->button() == Qt::LeftButton)) {
        requestMinimize();
        return;
    } else if (m_core.maximizeButtonRect().contains(pt)) {
        requestToggleMaximization(event->buttons());
        return;
    }
    KDecoration3::Decoration::mousePressEvent(event);
}

void QuartzDecorationPlugin::mouseReleaseEvent(QMouseEvent *event)
{
    KDecoration3::Decoration::mouseReleaseEvent(event);
}

} // namespace Quartz::Compositor

#include "quartzdecoration_plugin.moc"
