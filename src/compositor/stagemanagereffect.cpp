#include "stagemanagereffect.h"

namespace Quartz::Compositor {

StageManagerEffect::StageManagerEffect(QObject *parent)
    : QObject(parent)
{
}

void StageManagerEffect::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        m_spatialMode = enabled ? SpatialMode::StageManager : SpatialMode::Normal;
        recalculateStripLayout();
        Q_EMIT stateChanged(m_enabled);
        Q_EMIT spatialModeChanged(static_cast<int>(m_spatialMode));
    }
}

void StageManagerEffect::toggleMissionControl()
{
    if (m_spatialMode == SpatialMode::MissionControl) {
        m_spatialMode = m_enabled ? SpatialMode::StageManager : SpatialMode::Normal;
    } else {
        m_spatialMode = SpatialMode::MissionControl;
    }
    Q_EMIT spatialModeChanged(static_cast<int>(m_spatialMode));
}

void StageManagerEffect::toggleAppExpose(const QString &/*activeAppId*/)
{
    if (m_spatialMode == SpatialMode::AppExpose) {
        m_spatialMode = m_enabled ? SpatialMode::StageManager : SpatialMode::Normal;
    } else {
        m_spatialMode = SpatialMode::AppExpose;
    }
    Q_EMIT spatialModeChanged(static_cast<int>(m_spatialMode));
}

void StageManagerEffect::activateStageGroup(const QString &groupId)
{
    if (m_stageGroups.contains(groupId)) {
        m_activeGroupId = groupId;
        recalculateStripLayout();
        Q_EMIT stageGroupActivated(groupId);
    }
}

void StageManagerEffect::addWindowToCurrentStage(const QString &windowInternalId)
{
    if (m_activeGroupId.isEmpty()) {
        m_activeGroupId = QStringLiteral("group_1");
    }

    WindowStageGroup &group = m_stageGroups[m_activeGroupId];
    group.groupId = m_activeGroupId;
    if (!group.windowIds.contains(windowInternalId)) {
        group.windowIds.append(windowInternalId);
    }

    recalculateStripLayout();
}

void StageManagerEffect::recalculateStripLayout()
{
    qreal startY = 60.0;
    qreal spacing = 16.0;
    qreal thumbnailHeight = 90.0;
    qreal thumbnailWidth = 140.0;

    int idx = 0;
    for (auto it = m_stageGroups.begin(); it != m_stageGroups.end(); ++it) {
        if (it.key() == m_activeGroupId) {
            it->stripThumbnailGeometry = QRectF();
            continue;
        }

        qreal y = startY + idx * (thumbnailHeight + spacing);
        it->stripThumbnailGeometry = QRectF(12.0, y, thumbnailWidth, thumbnailHeight);
        it->perspectiveAngleY = m_perspectiveAngleY;
        it->scaleFactor = m_scaleFactor;
        idx++;
    }
}

QMatrix4x4 StageManagerEffect::calculateStageTransform(const QRectF &windowGeo, int stageIndex) const
{
    QMatrix4x4 mat;
    if (!m_enabled || stageIndex < 0) {
        return mat;
    }

    qreal targetX = 24.0;
    qreal targetY = 70.0 + stageIndex * 110.0;

    mat.translate(targetX, targetY, 0.0);
    mat.scale(m_scaleFactor, m_scaleFactor, 1.0);
    mat.rotate(m_perspectiveAngleY, 0.0, 1.0, 0.0);
    mat.translate(-windowGeo.x(), -windowGeo.y(), 0.0);

    return mat;
}

qreal StageManagerEffect::calculateSquircleDistance(qreal x, qreal y, qreal radius, qreal power)
{
    if (radius <= 0.0) {
        return 0.0;
    }
    qreal nx = qAbs(x) / radius;
    qreal ny = qAbs(y) / radius;
    // (|x/r|^n + |y/r|^n)^(1/n)
    return qPow(qPow(nx, power) + qPow(ny, power), 1.0 / power);
}

DropShadowParams StageManagerEffect::shadowForWindow(bool isKeyWindow)
{
    if (isKeyWindow) {
        // Ventana activa: mayor radio de difusión, opacidad pronunciada y elevación
        return DropShadowParams{ 32.0, 0.35, 12.0 };
    } else {
        // Ventana inactiva: sombra suave y cercana
        return DropShadowParams{ 14.0, 0.18, 4.0 };
    }
}

} // namespace Quartz::Compositor
