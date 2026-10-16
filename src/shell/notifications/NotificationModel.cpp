#include "NotificationModel.h"
#include <QDateTime>

namespace Quartz::Shell {

NotificationModel::NotificationModel(NotificationServer *server, QObject *parent)
    : QObject(parent)
    , m_server(server)
{
    if (m_server) {
        connect(m_server, &NotificationServer::notificationReceived,
                this, &NotificationModel::handleNotificationReceived);
        connect(m_server, &NotificationServer::notificationDismissed,
                this, &NotificationModel::handleNotificationDismissed);
    }
}

QVariantList NotificationModel::activeBanners() const
{
    QVariantList list;
    for (const auto &item : m_activeList) {
        list.append(itemToMap(item));
    }
    return list;
}

QVariantList NotificationModel::history() const
{
    QVariantList list;
    for (const auto &item : m_historyList) {
        list.append(itemToMap(item));
    }
    return list;
}

void NotificationModel::setCenterVisible(bool visible)
{
    if (m_centerVisible != visible) {
        m_centerVisible = visible;
        if (m_centerVisible) {
            // Marcar como leídas al abrir el centro de notificaciones
            for (auto &item : m_historyList) {
                item.isRead = true;
            }
            Q_EMIT unreadCountChanged();
            Q_EMIT historyChanged();
        }
        Q_EMIT centerVisibleChanged();
    }
}

int NotificationModel::unreadCount() const
{
    int count = 0;
    for (const auto &item : m_historyList) {
        if (!item.isRead) {
            count++;
        }
    }
    return count;
}

void NotificationModel::dismiss(uint id)
{
    // Eliminar de los banners activos
    for (int i = 0; i < m_activeList.size(); ++i) {
        if (m_activeList[i].id == id) {
            m_activeList.removeAt(i);
            Q_EMIT activeBannersChanged();
            break;
        }
    }

    if (m_server) {
        m_server->CloseNotification(id);
    }
}

void NotificationModel::invokeAction(uint id, const QString &actionKey)
{
    if (m_server) {
        Q_EMIT m_server->ActionInvoked(id, actionKey);
    }
    dismiss(id);
}

void NotificationModel::clearAll()
{
    m_activeList.clear();
    m_historyList.clear();
    Q_EMIT activeBannersChanged();
    Q_EMIT historyChanged();
    Q_EMIT unreadCountChanged();
}

void NotificationModel::toggleCenter()
{
    setCenterVisible(!m_centerVisible);
}

void NotificationModel::sendTestNotification(const QString &title, const QString &body)
{
    if (m_server) {
        m_server->Notify(QStringLiteral("Quartz"), 0, QStringLiteral("dialog-information"),
                         title, body, QStringList{}, QVariantMap{}, 5000);
    }
}

void NotificationModel::handleNotificationReceived(const NotificationItem &item)
{
    // Si reemplaza una notificación existente, sustituirla
    bool replacedActive = false;
    for (int i = 0; i < m_activeList.size(); ++i) {
        if (m_activeList[i].id == item.id) {
            m_activeList[i] = item;
            replacedActive = true;
            break;
        }
    }
    if (!replacedActive) {
        m_activeList.prepend(item);
    }

    bool replacedHistory = false;
    for (int i = 0; i < m_historyList.size(); ++i) {
        if (m_historyList[i].id == item.id) {
            m_historyList[i] = item;
            replacedHistory = true;
            break;
        }
    }
    if (!replacedHistory) {
        m_historyList.prepend(item);
    }

    Q_EMIT activeBannersChanged();
    Q_EMIT historyChanged();
    Q_EMIT unreadCountChanged();

    // Auto-cierre del banner activo tras expireTimeout
    if (item.expireTimeout > 0) {
        uint targetId = item.id;
        QTimer::singleShot(item.expireTimeout, this, [this, targetId]() {
            for (int i = 0; i < m_activeList.size(); ++i) {
                if (m_activeList[i].id == targetId) {
                    m_activeList.removeAt(i);
                    Q_EMIT activeBannersChanged();
                    break;
                }
            }
        });
    }
}

void NotificationModel::handleNotificationDismissed(uint id)
{
    for (int i = 0; i < m_activeList.size(); ++i) {
        if (m_activeList[i].id == id) {
            m_activeList.removeAt(i);
            Q_EMIT activeBannersChanged();
            break;
        }
    }
}

QVariantMap NotificationModel::itemToMap(const NotificationItem &item) const
{
    QVariantMap map;
    map[QStringLiteral("id")] = item.id;
    map[QStringLiteral("appName")] = item.appName;
    map[QStringLiteral("appIcon")] = item.appIcon;
    map[QStringLiteral("summary")] = item.summary;
    map[QStringLiteral("body")] = item.body;
    map[QStringLiteral("actions")] = item.actions;
    map[QStringLiteral("time")] = item.timestamp.toString(QStringLiteral("hh:mm"));
    map[QStringLiteral("isRead")] = item.isRead;
    return map;
}

} // namespace Quartz::Shell
