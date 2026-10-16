#include "stagemanagereffect.h"
#include <QDBusConnection>

namespace Quartz::Compositor {

StageManagerEffect::StageManagerEffect(QObject *parent)
    : QObject(parent)
{
    if (QDBusConnection::sessionBus().isConnected()) {
        QDBusConnection::sessionBus().registerObject(
            QStringLiteral("/org/quartz/Compositor/StageManager"),
            this,
            QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals
        );
    }
}

StageManagerEffect::~StageManagerEffect()
{
    if (QDBusConnection::sessionBus().isConnected()) {
        QDBusConnection::sessionBus().unregisterObject(QStringLiteral("/org/quartz/Compositor/StageManager"));
    }
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

QMatrix4x4 StageManagerEffect::calculateMissionControlTransform(const QRectF &windowGeo, int windowIndex, int totalWindows, const QRectF &screenGeo) const
{
    QMatrix4x4 mat;
    if (totalWindows <= 0 || windowIndex < 0 || windowIndex >= totalWindows || windowGeo.isEmpty() || screenGeo.isEmpty()) {
        return mat;
    }

    int cols = qCeil(qSqrt(totalWindows));
    int rows = qCeil(static_cast<qreal>(totalWindows) / cols);

    qreal marginX = 40.0;
    qreal marginTop = 120.0;
    qreal marginBottom = 80.0;

    qreal usableW = qMax(100.0, screenGeo.width() - 2 * marginX);
    qreal usableH = qMax(100.0, screenGeo.height() - marginTop - marginBottom);

    qreal cellW = usableW / cols;
    qreal cellH = usableH / rows;

    int r = windowIndex / cols;
    int c = windowIndex % cols;

    int itemsInThisRow = (r == rows - 1) ? (totalWindows - r * cols) : cols;
    qreal rowStartX = screenGeo.x() + marginX + (usableW - itemsInThisRow * cellW) / 2.0;

    qreal cellX = rowStartX + c * cellW;
    qreal cellY = screenGeo.y() + marginTop + r * cellH;

    qreal padding = 16.0;
    qreal maxW = qMax(10.0, cellW - 2 * padding);
    qreal maxH = qMax(10.0, cellH - 2 * padding);

    qreal scaleX = maxW / windowGeo.width();
    qreal scaleY = maxH / windowGeo.height();
    qreal scale = qMin(scaleX, scaleY);
    scale = qMin(scale, 0.85);

    qreal scaledW = windowGeo.width() * scale;
    qreal scaledH = windowGeo.height() * scale;

    qreal targetX = cellX + (cellW - scaledW) / 2.0;
    qreal targetY = cellY + (cellH - scaledH) / 2.0;

    mat.translate(targetX, targetY, 0.0);
    mat.scale(scale, scale, 1.0);
    mat.translate(-windowGeo.x(), -windowGeo.y(), 0.0);

    return mat;
}

QMatrix4x4 StageManagerEffect::calculateAppExposeTransform(const QRectF &windowGeo, int windowIndex, int totalWindows, const QRectF &screenGeo) const
{
    QMatrix4x4 mat;
    if (totalWindows <= 0 || windowIndex < 0 || windowIndex >= totalWindows || windowGeo.isEmpty() || screenGeo.isEmpty()) {
        return mat;
    }

    int cols = qCeil(qSqrt(totalWindows));
    int rows = qCeil(static_cast<qreal>(totalWindows) / cols);

    qreal marginX = 60.0;
    qreal marginTop = 80.0;
    qreal marginBottom = 80.0;

    qreal usableW = qMax(100.0, screenGeo.width() - 2 * marginX);
    qreal usableH = qMax(100.0, screenGeo.height() - marginTop - marginBottom);

    qreal cellW = usableW / cols;
    qreal cellH = usableH / rows;

    int r = windowIndex / cols;
    int c = windowIndex % cols;

    int itemsInThisRow = (r == rows - 1) ? (totalWindows - r * cols) : cols;
    qreal rowStartX = screenGeo.x() + marginX + (usableW - itemsInThisRow * cellW) / 2.0;

    qreal cellX = rowStartX + c * cellW;
    qreal cellY = screenGeo.y() + marginTop + r * cellH;

    qreal padding = 20.0;
    qreal maxW = qMax(10.0, cellW - 2 * padding);
    qreal maxH = qMax(10.0, cellH - 2 * padding);

    qreal scaleX = maxW / windowGeo.width();
    qreal scaleY = maxH / windowGeo.height();
    qreal scale = qMin(scaleX, scaleY);
    scale = qMin(scale, 0.90);

    qreal scaledW = windowGeo.width() * scale;
    qreal scaledH = windowGeo.height() * scale;

    qreal targetX = cellX + (cellW - scaledW) / 2.0;
    qreal targetY = cellY + (cellH - scaledH) / 2.0;

    mat.translate(targetX, targetY, 0.0);
    mat.scale(scale, scale, 1.0);
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
