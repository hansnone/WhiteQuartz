#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QTimer>
#include "NotificationServer.h"

namespace Quartz::Shell {

class NotificationModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList activeBanners READ activeBanners NOTIFY activeBannersChanged)
    Q_PROPERTY(QVariantList history READ history NOTIFY historyChanged)
    Q_PROPERTY(bool centerVisible READ centerVisible WRITE setCenterVisible NOTIFY centerVisibleChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)

public:
    explicit NotificationModel(NotificationServer *server, QObject *parent = nullptr);

    QVariantList activeBanners() const;
    QVariantList history() const;
    bool centerVisible() const { return m_centerVisible; }
    void setCenterVisible(bool visible);
    int unreadCount() const;

    Q_INVOKABLE void dismiss(uint id);
    Q_INVOKABLE void invokeAction(uint id, const QString &actionKey);
    Q_INVOKABLE void clearAll();
    Q_INVOKABLE void toggleCenter();
    Q_INVOKABLE void sendTestNotification(const QString &title, const QString &body);

Q_SIGNALS:
    void activeBannersChanged();
    void historyChanged();
    void centerVisibleChanged();
    void unreadCountChanged();

private Q_SLOTS:
    void handleNotificationReceived(const NotificationItem &item);
    void handleNotificationDismissed(uint id);

private:
    NotificationServer *m_server;
    QList<NotificationItem> m_activeList;
    QList<NotificationItem> m_historyList;
    bool m_centerVisible = false;

    QVariantMap itemToMap(const NotificationItem &item) const;
};

} // namespace Quartz::Shell
