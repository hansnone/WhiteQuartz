#pragma once

#include <QObject>
#include <QStringList>
#include <QtDBus/QDBusConnection>

namespace Quartz::Shell {

class StatusNotifierWatcher : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.StatusNotifierWatcher")
    Q_PROPERTY(QStringList RegisteredStatusNotifierItems READ registeredItems NOTIFY itemsChanged)
    Q_PROPERTY(bool IsStatusNotifierHostRegistered READ isHostRegistered CONSTANT)
    Q_PROPERTY(int ProtocolVersion READ protocolVersion CONSTANT)

public:
    explicit StatusNotifierWatcher(QObject *parent = nullptr);
    ~StatusNotifierWatcher() override;

    bool registerService();

    QStringList registeredItems() const { return m_items; }
    bool isHostRegistered() const { return true; }
    int protocolVersion() const { return 0; }

public Q_SLOTS:
    void RegisterStatusNotifierItem(const QString &serviceOrPath);
    void RegisterStatusNotifierHost(const QString &service);

Q_SIGNALS:
    void StatusNotifierItemRegistered(const QString &serviceOrPath);
    void StatusNotifierItemUnregistered(const QString &serviceOrPath);
    void StatusNotifierHostRegistered();
    void itemsChanged();

private:
    QStringList m_items;
};

} // namespace Quartz::Shell
