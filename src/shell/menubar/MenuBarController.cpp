#include "MenuBarController.h"
#include <QProcess>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusArgument>
#include <QDebug>

namespace Quartz::Shell {

MenuBarController::MenuBarController(QObject *parent)
    : QObject(parent)
{
    updateClock();
    connect(&m_clockTimer, &QTimer::timeout, this, &MenuBarController::updateClock);
    m_clockTimer.start(1000);

    setDefaultMenus();

    m_registrar.registerService();
    connect(&m_registrar, &AppMenuRegistrar::menuRegistered, this, &MenuBarController::onWindowMenuRegistered);
    connect(&m_registrar, &AppMenuRegistrar::menuUnregistered, this, &MenuBarController::onWindowMenuUnregistered);

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (bus.isConnected()) {
        bus.registerObject(QStringLiteral("/org/quartz/Shell/MenuBar"), this,
                           QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals);
    }
}

void MenuBarController::updateClock()
{
    QString now = QDateTime::currentDateTime().toString(QStringLiteral("ddd d MMM  HH:mm"));
    if (m_formattedTime != now) {
        m_formattedTime = now;
        Q_EMIT timeChanged();
    }
}

void MenuBarController::setActiveAppTitle(const QString &title)
{
    if (m_activeAppTitle != title) {
        m_activeAppTitle = title;
        Q_EMIT activeAppTitleChanged();
    }
}

void MenuBarController::setAltPressed(bool pressed)
{
    if (m_isAltPressed != pressed) {
        m_isAltPressed = pressed;
        Q_EMIT altPressedChanged(m_isAltPressed);
    }
}

void MenuBarController::toggleNotificationCenter()
{
    Q_EMIT notificationCenterToggled();
}

void MenuBarController::triggerAppleMenuAction(const QString &actionId)
{
    if (actionId == QStringLiteral("about")) {
        QProcess::startDetached(QStringLiteral("quartz-settings"), { QStringLiteral("--pane"), QStringLiteral("general") });
    } else if (actionId == QStringLiteral("settings")) {
        QProcess::startDetached(QStringLiteral("quartz-settings"), {});
    } else if (actionId == QStringLiteral("sleep")) {
        QProcess::startDetached(QStringLiteral("systemctl"), { QStringLiteral("suspend") });
    } else if (actionId == QStringLiteral("reboot")) {
        QProcess::startDetached(QStringLiteral("systemctl"), { QStringLiteral("reboot") });
    } else if (actionId == QStringLiteral("shutdown")) {
        QProcess::startDetached(QStringLiteral("systemctl"), { QStringLiteral("poweroff") });
    } else if (actionId == QStringLiteral("lock")) {
        QDBusMessage msg = QDBusMessage::createMethodCall(
            QStringLiteral("org.quartz.Session"),
            QStringLiteral("/org/quartz/Session"),
            QStringLiteral("org.quartz.Session"),
            QStringLiteral("lock_screen")
        );
        QDBusConnection::sessionBus().send(msg);
        QProcess::startDetached(QStringLiteral("loginctl"), { QStringLiteral("lock-session") });
    } else if (actionId == QStringLiteral("logout")) {
        QDBusMessage msg = QDBusMessage::createMethodCall(
            QStringLiteral("org.quartz.Session"),
            QStringLiteral("/org/quartz/Session"),
            QStringLiteral("org.quartz.Session"),
            QStringLiteral("request_logout")
        );
        QDBusConnection::sessionBus().send(msg);
        QProcess::startDetached(QStringLiteral("loginctl"), { QStringLiteral("terminate-session"), QStringLiteral("") });
    } else if (actionId == QStringLiteral("forcequit")) {
        // Lanzar diálogo de forzar salida de aplicaciones
        QProcess::startDetached(QStringLiteral("xkill"), {});
    }
}

void MenuBarController::setDefaultMenus()
{
    m_currentMenuService.clear();
    m_currentMenuPath.clear();

    auto makeMenu = [](const QString &label, const QVariantList &items) {
        QVariantMap map;
        map[QStringLiteral("label")] = label;
        map[QStringLiteral("children")] = items;
        return map;
    };

    auto makeItem = [](int id, const QString &label, const QString &shortcut = QString()) {
        QVariantMap item;
        item[QStringLiteral("id")] = id;
        item[QStringLiteral("label")] = label;
        item[QStringLiteral("shortcut")] = shortcut;
        return item;
    };

    QVariantList menus;

    // 1. Archivo
    QVariantList fileItems;
    fileItems.append(makeItem(101, QStringLiteral("Nueva ventana"), QStringLiteral("Ctrl+N")));
    fileItems.append(makeItem(102, QStringLiteral("Abrir..."), QStringLiteral("Ctrl+O")));
    fileItems.append(makeItem(103, QStringLiteral("Cerrar ventana"), QStringLiteral("Ctrl+W")));
    menus.append(makeMenu(QStringLiteral("Archivo"), fileItems));

    // 2. Edición
    QVariantList editItems;
    editItems.append(makeItem(201, QStringLiteral("Deshacer"), QStringLiteral("Ctrl+Z")));
    editItems.append(makeItem(202, QStringLiteral("Rehacer"), QStringLiteral("Ctrl+Shift+Z")));
    editItems.append(makeItem(203, QStringLiteral("Cortar"), QStringLiteral("Ctrl+X")));
    editItems.append(makeItem(204, QStringLiteral("Copiar"), QStringLiteral("Ctrl+C")));
    editItems.append(makeItem(205, QStringLiteral("Pegar"), QStringLiteral("Ctrl+V")));
    editItems.append(makeItem(206, QStringLiteral("Seleccionar todo"), QStringLiteral("Ctrl+A")));
    menus.append(makeMenu(QStringLiteral("Edición"), editItems));

    // 3. Visualización
    QVariantList viewItems;
    viewItems.append(makeItem(301, QStringLiteral("Como iconos"), QStringLiteral("Ctrl+1")));
    viewItems.append(makeItem(302, QStringLiteral("Como lista"), QStringLiteral("Ctrl+2")));
    viewItems.append(makeItem(303, QStringLiteral("Como columnas"), QStringLiteral("Ctrl+3")));
    viewItems.append(makeItem(304, QStringLiteral("Como galería"), QStringLiteral("Ctrl+4")));
    menus.append(makeMenu(QStringLiteral("Visualización"), viewItems));

    // 4. Ventana
    QVariantList winItems;
    winItems.append(makeItem(401, QStringLiteral("Minimizar"), QStringLiteral("Ctrl+M")));
    winItems.append(makeItem(402, QStringLiteral("Zoom")));
    winItems.append(makeItem(403, QStringLiteral("Traer todo al frente")));
    menus.append(makeMenu(QStringLiteral("Ventana"), winItems));

    // 5. Ayuda
    QVariantList helpItems;
    helpItems.append(makeItem(501, QStringLiteral("Buscar ayuda")));
    helpItems.append(makeItem(502, QStringLiteral("Ayuda de WhiteQuartz")));
    menus.append(makeMenu(QStringLiteral("Ayuda"), helpItems));

    m_activeAppMenus = menus;
    Q_EMIT activeAppMenusChanged();
}

void MenuBarController::fetchDBusMenu(const QString &service, const QString &path)
{
    m_currentMenuService = service;
    m_currentMenuPath = path;

    QDBusMessage msg = QDBusMessage::createMethodCall(
        service,
        path,
        QStringLiteral("com.canonical.dbusmenu"),
        QStringLiteral("GetLayout")
    );
    msg << 0 << 2 << QStringList();

    QDBusPendingCall pcall = QDBusConnection::sessionBus().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *w) {
        w->deleteLater();
        QDBusMessage reply = w->reply();
        if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().size() < 2) {
            setDefaultMenus();
            return;
        }

        // Si la llamada fue exitosa, parsear los menús devueltos
        QDBusArgument arg = qvariant_cast<QDBusArgument>(reply.arguments().at(1));
        int rootId = 0;
        QVariantMap rootProps;
        QList<QDBusArgument> rootChildren;

        arg.beginStructure();
        arg >> rootId >> rootProps;
        arg.beginArray();
        QVariantList parsedTopMenus;

        while (!arg.atEnd()) {
            int topId = 0;
            QVariantMap topProps;
            arg.beginStructure();
            arg >> topId >> topProps;

            QString topLabel = topProps.value(QStringLiteral("label")).toString().replace(QStringLiteral("_"), QString());
            if (topLabel.isEmpty()) {
                topLabel = QStringLiteral("Menú");
            }

            QVariantList subItems;
            arg.beginArray();
            while (!arg.atEnd()) {
                int itemId = 0;
                QVariantMap itemProps;
                arg.beginStructure();
                arg >> itemId >> itemProps;
                QString itemLabel = itemProps.value(QStringLiteral("label")).toString().replace(QStringLiteral("_"), QString());
                QVariantMap itemMap;
                itemMap[QStringLiteral("id")] = itemId;
                itemMap[QStringLiteral("label")] = itemLabel;
                subItems.append(itemMap);
                arg.endStructure();
            }
            arg.endArray();

            QVariantMap menuMap;
            menuMap[QStringLiteral("id")] = topId;
            menuMap[QStringLiteral("label")] = topLabel;
            menuMap[QStringLiteral("children")] = subItems;
            parsedTopMenus.append(menuMap);

            arg.endStructure();
        }
        arg.endArray();
        arg.endStructure();

        if (!parsedTopMenus.isEmpty()) {
            m_activeAppMenus = parsedTopMenus;
            Q_EMIT activeAppMenusChanged();
        } else {
            setDefaultMenus();
        }
    });
}

void MenuBarController::triggerMenuAction(int id)
{
    if (m_currentMenuService.isEmpty() || m_currentMenuPath.isEmpty()) {
        return;
    }

    QDBusMessage msg = QDBusMessage::createMethodCall(
        m_currentMenuService,
        m_currentMenuPath,
        QStringLiteral("com.canonical.dbusmenu"),
        QStringLiteral("Event")
    );

    msg << id << QStringLiteral("clicked") << QVariant(0) << static_cast<uint>(QDateTime::currentMSecsSinceEpoch() / 1000);
    QDBusConnection::sessionBus().send(msg);
}

void MenuBarController::onActiveWindowChanged(const QString &appId, const QString &title,
                                              const QString &dbusMenuService,
                                              const QString &dbusMenuPath)
{
    Q_UNUSED(appId);
    if (!title.isEmpty()) {
        setActiveAppTitle(title);
    }

    if (!dbusMenuService.isEmpty() && !dbusMenuPath.isEmpty()) {
        fetchDBusMenu(dbusMenuService, dbusMenuPath);
    } else {
        setDefaultMenus();
    }
}

void MenuBarController::onWindowMenuRegistered(uint windowId, const QString &service, const QString &menuPath)
{
    Q_UNUSED(windowId);
    fetchDBusMenu(service, menuPath);
}

void MenuBarController::onWindowMenuUnregistered(uint windowId)
{
    Q_UNUSED(windowId);
    setDefaultMenus();
}

} // namespace Quartz::Shell
