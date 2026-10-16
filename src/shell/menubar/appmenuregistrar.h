#pragma once

#include <QObject>
#include <QDBusContext>
#include <QDBusObjectPath>
#include <QHash>
#include <QString>

namespace Quartz::Shell {

struct RegisteredMenu {
    QString service;
    QDBusObjectPath menuObjectPath;
};

class AppMenuRegistrar : public QObject, protected QDBusContext {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.AppMenu.Registrar")

public:
    explicit AppMenuRegistrar(QObject *parent = nullptr);
    ~AppMenuRegistrar() override = default;

    bool registerService();

public Q_SLOTS:
    Q_SCRIPTABLE void RegisterWindow(uint windowId, const QDBusObjectPath &menuObjectPath);
    Q_SCRIPTABLE void UnregisterWindow(uint windowId);
    Q_SCRIPTABLE QString GetMenuForWindow(uint windowId, QDBusObjectPath &menuObjectPath);

Q_SIGNALS:
    Q_SCRIPTABLE void WindowRegistered(uint windowId, const QString &service, const QDBusObjectPath &menuObjectPath);
    Q_SCRIPTABLE void WindowUnregistered(uint windowId);

    void menuRegistered(uint windowId, const QString &service, const QString &menuPath);
    void menuUnregistered(uint windowId);

private:
    QHash<uint, RegisteredMenu> m_registeredMenus;
};

} // namespace Quartz::Shell
