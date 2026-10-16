#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QVariantList>
#include <QDBusMessage>
#include "appmenuregistrar.h"

namespace Quartz::Shell {

class MenuBarController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString formattedTime READ formattedTime NOTIFY timeChanged)
    Q_PROPERTY(QString activeAppTitle READ activeAppTitle NOTIFY activeAppTitleChanged)
    Q_PROPERTY(bool isAltPressed READ isAltPressed NOTIFY altPressedChanged)
    Q_PROPERTY(QVariantList activeAppMenus READ activeAppMenus NOTIFY activeAppMenusChanged)

public:
    explicit MenuBarController(QObject *parent = nullptr);
    ~MenuBarController() override = default;

    QString formattedTime() const { return m_formattedTime; }
    QString activeAppTitle() const { return m_activeAppTitle; }
    bool isAltPressed() const { return m_isAltPressed; }
    QVariantList activeAppMenus() const { return m_activeAppMenus; }

public Q_SLOTS:
    void setActiveAppTitle(const QString &title);
    void triggerAppleMenuAction(const QString &actionId);
    void setAltPressed(bool pressed);
    void toggleNotificationCenter();

    Q_INVOKABLE void triggerMenuAction(int id);
    Q_INVOKABLE void onActiveWindowChanged(const QString &appId, const QString &title,
                                           const QString &dbusMenuService = QString(),
                                           const QString &dbusMenuPath = QString());
    void onWindowMenuRegistered(uint windowId, const QString &service, const QString &menuPath);
    void onWindowMenuUnregistered(uint windowId);

Q_SIGNALS:
    void timeChanged();
    void activeAppTitleChanged();
    void altPressedChanged(bool pressed);
    void notificationCenterToggled();
    void activeAppMenusChanged();

private Q_SLOTS:
    void updateClock();

private:
    void setDefaultMenus();
    void fetchDBusMenu(const QString &service, const QString &path);

    QString m_formattedTime;
    QString m_activeAppTitle = QStringLiteral("Finder");
    bool m_isAltPressed = false;
    QTimer m_clockTimer;

    QVariantList m_activeAppMenus;
    QString m_currentMenuService;
    QString m_currentMenuPath;

    AppMenuRegistrar m_registrar;
};

} // namespace Quartz::Shell
