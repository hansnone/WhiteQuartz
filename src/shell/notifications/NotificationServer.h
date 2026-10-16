#pragma once

#include <QObject>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QStringList>
#include <QVariantMap>
#include <QDateTime>

namespace Quartz::Shell {

struct NotificationItem {
    uint id = 0;
    QString appName;
    QString appIcon;
    QString summary;
    QString body;
    QStringList actions;
    QVariantMap hints;
    int expireTimeout = 5000;
    QDateTime timestamp;
    bool isRead = false;
};

class NotificationServer : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")

public:
    explicit NotificationServer(QObject *parent = nullptr);
    ~NotificationServer() override;

    bool registerService();

public Q_SLOTS:
    QStringList GetCapabilities();

    uint Notify(const QString &app_name,
                uint replaces_id,
                const QString &app_icon,
                const QString &summary,
                const QString &body,
                const QStringList &actions,
                const QVariantMap &hints,
                int expire_timeout);

    void CloseNotification(uint id);

    QString GetServerInformation(QString &vendor, QString &version, QString &spec_version);

Q_SIGNALS:
    void NotificationClosed(uint id, uint reason);
    void ActionInvoked(uint id, const QString &action_key);

    void notificationReceived(const NotificationItem &item);
    void notificationDismissed(uint id);

private:
    uint m_nextId = 1;
};

} // namespace Quartz::Shell
