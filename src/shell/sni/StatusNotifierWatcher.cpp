#include "StatusNotifierWatcher.h"
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusServiceWatcher>
#include <QDebug>

namespace Quartz::Shell {

StatusNotifierWatcher::StatusNotifierWatcher(QObject *parent)
    : QObject(parent)
{
}

StatusNotifierWatcher::~StatusNotifierWatcher()
{
    QDBusConnection::sessionBus().unregisterService(QStringLiteral("org.kde.StatusNotifierWatcher"));
    QDBusConnection::sessionBus().unregisterObject(QStringLiteral("/StatusNotifierWatcher"));
}

bool StatusNotifierWatcher::registerService()
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) return false;

    if (!bus.registerObject(QStringLiteral("/StatusNotifierWatcher"), this,
                            QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllProperties)) {
        qWarning() << "[StatusNotifierWatcher] Error registrando objeto D-Bus:" << bus.lastError().message();
        return false;
    }

    bus.registerService(QStringLiteral("org.kde.StatusNotifierWatcher"));
    return true;
}

void StatusNotifierWatcher::RegisterStatusNotifierItem(const QString &serviceOrPath)
{
    QString item = serviceOrPath;
    if (!item.startsWith(QLatin1Char('/'))) {
        // Formato servicio/objeto
        if (!item.contains(QLatin1Char('/'))) {
            item += QStringLiteral("/StatusNotifierItem");
        }
    }

    if (!m_items.contains(item)) {
        m_items.append(item);
        Q_EMIT StatusNotifierItemRegistered(item);
        Q_EMIT itemsChanged();

        // Monitorizar caída del servicio para desregistrar
        QString service = item.section(QLatin1Char('/'), 0, 0);
        auto *watcher = new QDBusServiceWatcher(service, QDBusConnection::sessionBus(),
                                                QDBusServiceWatcher::WatchForUnregistration, this);
        connect(watcher, &QDBusServiceWatcher::serviceUnregistered, this, [this, item, watcher]() {
            watcher->deleteLater();
            if (m_items.removeOne(item)) {
                Q_EMIT StatusNotifierItemUnregistered(item);
                Q_EMIT itemsChanged();
            }
        });
    }
}

void StatusNotifierWatcher::RegisterStatusNotifierHost(const QString &service)
{
    Q_UNUSED(service);
    Q_EMIT StatusNotifierHostRegistered();
}

} // namespace Quartz::Shell
