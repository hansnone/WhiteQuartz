#pragma once

#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QtMath>
#include "../../style/quartzmetrics.h"

namespace Quartz::Shell {

class DockController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int baseIconSize READ baseIconSize CONSTANT)
    Q_PROPERTY(int maxMagnifiedSize READ maxMagnifiedSize CONSTANT)

public:
    explicit DockController(QObject *parent = nullptr);

    int baseIconSize() const { return StyleMetrics::DockBaseIconSize; }
    int maxMagnifiedSize() const { return StyleMetrics::DockMaxMagnifiedSize; }

    Q_INVOKABLE qreal calculateGaussianMagnification(qreal itemCenterDistance, qreal sigma = 65.0) const;
    Q_INVOKABLE qreal calculateParabolicBounce(qreal progress, qreal bounceHeight = 22.0) const;
    Q_INVOKABLE QVariantList defaultApps() const;
    Q_INVOKABLE void launchApp(const QString &cmd);
    Q_INVOKABLE void requestAppAttention(const QString &cmd);

Q_SIGNALS:
    void appBouncingRequested(const QString &cmd);
};

} // namespace Quartz::Shell
