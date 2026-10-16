#pragma once

#include <QObject>
#include <QRect>
#include <QString>
#include <QColor>
#include <QPoint>
#include <QTimer>
#include "../style/quartzmetrics.h"

class QPainter;

namespace Quartz::Compositor {

enum class TrafficLightHoverState {
    None,
    CloseHovered,
    MinimizeHovered,
    ZoomHovered
};

enum class WindowTileMode {
    None,
    FullScreen,
    TileLeftHalf,
    TileRightHalf
};

class QuartzDecoration : public QObject {
    Q_OBJECT

public:
    explicit QuartzDecoration(QObject *parent = nullptr);

    bool isUnifiedToolbar() const { return m_unifiedToolbar; }
    void setUnifiedToolbar(bool unified);

    int titleBarHeight() const;
    QRect closeButtonRect() const;
    QRect minimizeButtonRect() const;
    QRect maximizeButtonRect() const;

    bool documentHasUnsavedChanges() const { return m_unsavedChanges; }
    void setDocumentHasUnsavedChanges(bool unsaved);

    TrafficLightHoverState hoverState() const { return m_hoverState; }
    void setHoverState(TrafficLightHoverState state);

    bool isTilingMenuVisible() const { return m_tilingMenuVisible; }

    void paintTrafficLights(QPainter *painter, const QRect &titleBarRect, TrafficLightHoverState hoverState, bool unsaved);
    void drawTitleBar(QPainter *painter, const QRect &titleBarRect, bool active, const QString &caption, const QColor &textColor = QColor(240, 240, 240));
    static QImage generateShadowImage(int width, int height, bool active);

public Q_SLOTS:
    void applyTileMode(WindowTileMode mode);
    void onZoomHoverEntered(const QPoint &globalPos);
    void onZoomHoverLeft();

Q_SIGNALS:
    void titleBarHeightChanged(int newHeight);
    void unsavedChangesChanged(bool unsaved);
    void hoverStateChanged(TrafficLightHoverState state);
    void tilingMenuRequested(const QPoint &globalPos);
    void tileModeApplied(WindowTileMode mode);

private Q_SLOTS:
    void onZoomHoverTimerTimeout();

private:
    bool m_unifiedToolbar = false;
    bool m_unsavedChanges = false;
    bool m_tilingMenuVisible = false;
    TrafficLightHoverState m_hoverState = TrafficLightHoverState::None;
    QPoint m_lastZoomPos;
    QTimer m_zoomHoverTimer;
};

} // namespace Quartz::Compositor
