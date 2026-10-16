#include "appmenuregistrar.h"
#include <QDBusConnection>
#include <QDBusMessage>

namespace Quartz::Shell {

AppMenuRegistrar::AppMenuRegistrar(QObject *parent)
    : QObject(parent)
{
}

bool AppMenuRegistrar::registerService()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        return false;
    }

    bus.registerService(QStringLiteral("com.canonical.AppMenu.Registrar"));
    return bus.registerObject(QStringLiteral("/com/canonical/AppMenu/Registrar"), this,
                              QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals);
}

void AppMenuRegistrar::RegisterWindow(uint windowId, const QDBusObjectPath &menuObjectPath)
{
    QString callerService = QStringLiteral("local");
    if (calledFromDBus()) {
        callerService = message().service();
        if (callerService.isEmpty()) {
            callerService = QStringLiteral("local");
        }
    }

    RegisteredMenu entry{ callerService, menuObjectPath };
    m_registeredMenus.insert(windowId, entry);

    Q_EMIT WindowRegistered(windowId, callerService, menuObjectPath);
    Q_EMIT menuRegistered(windowId, callerService, menuObjectPath.path());
}

void AppMenuRegistrar::UnregisterWindow(uint windowId)
{
    if (m_registeredMenus.remove(windowId) > 0) {
        Q_EMIT WindowUnregistered(windowId);
        Q_EMIT menuUnregistered(windowId);
    }
}

QString AppMenuRegistrar::GetMenuForWindow(uint windowId, QDBusObjectPath &menuObjectPath)
{
    if (m_registeredMenus.contains(windowId)) {
        const auto &entry = m_registeredMenus.value(windowId);
        menuObjectPath = entry.menuObjectPath;
        return entry.service;
    }
    return QString();
}

} // namespace Quartz::Shell
