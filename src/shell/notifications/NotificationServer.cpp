#include "NotificationServer.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QRegularExpression>
#include <QDebug>

namespace Quartz::Shell {

NotificationServer::NotificationServer(QObject *parent)
    : QObject(parent)
{
}

NotificationServer::~NotificationServer()
{
    QDBusConnection::sessionBus().unregisterService(QStringLiteral("org.freedesktop.Notifications"));
    QDBusConnection::sessionBus().unregisterObject(QStringLiteral("/org/freedesktop/Notifications"));
}

bool NotificationServer::registerService()
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        qWarning() << "[NotificationServer] No hay conexión al bus de sesión D-Bus";
        return false;
    }

    if (!bus.registerObject(QStringLiteral("/org/freedesktop/Notifications"), this,
                            QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qWarning() << "[NotificationServer] Error al registrar objeto D-Bus:" << bus.lastError().message();
        return false;
    }

    if (!bus.registerService(QStringLiteral("org.freedesktop.Notifications"))) {
        qWarning() << "[NotificationServer] No se pudo reclamar el servicio org.freedesktop.Notifications (quizás otro demonio esté activo):"
                   << bus.lastError().message();
        // No abortar: continuamos si ya está registrado o en tests
    }

    return true;
}

QStringList NotificationServer::GetCapabilities()
{
    return QStringList{
        QStringLiteral("body"),
        QStringLiteral("body-hyperlinks"),
        QStringLiteral("body-markup"),
        QStringLiteral("icon-static"),
        QStringLiteral("actions"),
        QStringLiteral("persistence")
    };
}

uint NotificationServer::Notify(const QString &app_name,
                                uint replaces_id,
                                const QString &app_icon,
                                const QString &summary,
                                const QString &body,
                                const QStringList &actions,
                                const QVariantMap &hints,
                                int expire_timeout)
{
    uint id = (replaces_id > 0) ? replaces_id : m_nextId++;

    NotificationItem item;
    item.id = id;
    item.appName = app_name.trimmed().isEmpty() ? QStringLiteral("Sistema") : app_name;
    item.appIcon = app_icon;
    
    // Saneamiento de seguridad de cadenas externas (regla 0.2.7)
    item.summary = summary;
    
    // Limpiar etiquetas de script o maliciosas en el cuerpo HTML
    QString cleanBody = body;
    cleanBody.remove(QRegularExpression(QStringLiteral("<script[^>]*>.*?</script>"), QRegularExpression::CaseInsensitiveOption));
    item.body = cleanBody;

    item.actions = actions;
    item.hints = hints;
    item.expireTimeout = (expire_timeout <= 0) ? 5000 : expire_timeout;
    item.timestamp = QDateTime::currentDateTime();
    item.isRead = false;

    Q_EMIT notificationReceived(item);
    return id;
}

void NotificationServer::CloseNotification(uint id)
{
    // Razón 3 = cerrado mediante llamada explícita a CloseNotification
    Q_EMIT NotificationClosed(id, 3);
    Q_EMIT notificationDismissed(id);
}

QString NotificationServer::GetServerInformation(QString &vendor, QString &version, QString &spec_version)
{
    vendor = QStringLiteral("QuartzDE");
    version = QStringLiteral("0.1.0");
    spec_version = QStringLiteral("1.2");
    return QStringLiteral("Quartz Notification Server");
}

} // namespace Quartz::Shell
