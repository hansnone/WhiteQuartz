#pragma once

#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QSet>
#include <QFileSystemWatcher>
#include <QtMath>
#include "../../style/quartzmetrics.h"

namespace Quartz::Shell {

class DockController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int baseIconSize READ baseIconSize CONSTANT)
    Q_PROPERTY(int maxMagnifiedSize READ maxMagnifiedSize CONSTANT)
    Q_PROPERTY(QVariantList apps READ apps NOTIFY appsChanged)
    Q_PROPERTY(QString trashIcon READ trashIcon NOTIFY trashIconChanged)

public:
    explicit DockController(QObject *parent = nullptr);
    ~DockController() override = default;

    int baseIconSize() const { return StyleMetrics::DockBaseIconSize; }
    int maxMagnifiedSize() const { return StyleMetrics::DockMaxMagnifiedSize; }
    QVariantList apps() const { return m_apps; }
    QString trashIcon() const { return m_trashIcon; }

    Q_INVOKABLE qreal calculateGaussianMagnification(qreal itemCenterDistance, qreal sigma = 65.0) const;
    Q_INVOKABLE qreal calculateParabolicBounce(qreal progress, qreal bounceHeight = 22.0) const;
    Q_INVOKABLE QVariantList defaultApps() const;
    Q_INVOKABLE QStringList parseCommandLine(const QString &cmd) const;
    Q_INVOKABLE void launchApp(const QString &cmd);
    Q_INVOKABLE void requestAppAttention(const QString &cmd);

    // Stacks y Papelera
    Q_INVOKABLE QVariantList listStackDirectory(const QString &dirPath) const;
    Q_INVOKABLE void updateTrashState();

    // Gestión contextual y ciclo de vida
    Q_INVOKABLE void quitApp(const QString &cmd);
    Q_INVOKABLE void forceQuitApp(const QString &cmd);
    Q_INVOKABLE void toggleKeepInDock(const QString &cmd);

public Q_SLOTS:
    void onWindowAdded(const QString &appId, const QString &title, const QString &icon, bool active);
    void onWindowRemoved(const QString &appId);
    void onActiveWindowChanged(const QString &appId, const QString &title);

Q_SIGNALS:
    void appBouncingRequested(const QString &cmd);
    void appsChanged();
    void trashIconChanged(const QString &icon);

private:
    void rebuildAppList();

    QVariantList m_apps;
    QSet<QString> m_runningAppIds;
    QFileSystemWatcher m_trashWatcher;
    QString m_trashIcon = QStringLiteral("user-trash");
};

} // namespace Quartz::Shell
