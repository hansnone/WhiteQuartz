#pragma once

#include <QObject>
#include <QHash>
#include <QVector>
#include <QRectF>
#include <QMatrix4x4>
#include <QString>
#include <QtMath>

namespace Quartz::Compositor {

struct DropShadowParams {
    qreal blurRadius;
    qreal opacity;
    qreal yOffset;
};

enum class SpatialMode {
    Normal,
    StageManager,
    MissionControl,
    AppExpose
};

struct WindowStageGroup {
    QString groupId;
    QVector<QString> windowIds;
    QRectF stripThumbnailGeometry;
    qreal perspectiveAngleY = 14.0;
    qreal scaleFactor = 0.18;
};

class StageManagerEffect : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.quartz.Compositor.StageManager")

public:
    explicit StageManagerEffect(QObject *parent = nullptr);
    ~StageManagerEffect() override;

    bool isEnabled() const { return m_enabled; }
    SpatialMode spatialMode() const { return m_spatialMode; }
    qreal perspectiveAngleY() const { return m_perspectiveAngleY; }
    qreal scaleFactor() const { return m_scaleFactor; }

    QMatrix4x4 calculateStageTransform(const QRectF &windowGeo, int stageIndex) const;
    static qreal calculateSquircleDistance(qreal x, qreal y, qreal radius, qreal power = 4.5);
    static DropShadowParams shadowForWindow(bool isKeyWindow);

public Q_SLOTS:
    Q_SCRIPTABLE void setEnabled(bool enabled);
    Q_SCRIPTABLE bool isEnabledSlot() const { return m_enabled; }
    Q_SCRIPTABLE void activateStageGroup(const QString &groupId);
    Q_SCRIPTABLE void addWindowToCurrentStage(const QString &windowInternalId);
    Q_SCRIPTABLE int activeStageCount() const { return m_stageGroups.size(); }

    // Gestión espacial con atajos Windows (Super+Up / Super+Down)
    Q_SCRIPTABLE void toggleMissionControl();
    Q_SCRIPTABLE void toggleAppExpose(const QString &activeAppId);

Q_SIGNALS:
    Q_SCRIPTABLE void stageGroupActivated(const QString &groupId);
    Q_SCRIPTABLE void stateChanged(bool enabled);
    Q_SCRIPTABLE void spatialModeChanged(int newMode);

private:
    void recalculateStripLayout();

    bool m_enabled = false;
    SpatialMode m_spatialMode = SpatialMode::Normal;
    qreal m_perspectiveAngleY = 14.0;
    qreal m_scaleFactor = 0.18;
    QString m_activeGroupId;
    QHash<QString, WindowStageGroup> m_stageGroups;
};

} // namespace Quartz::Compositor
