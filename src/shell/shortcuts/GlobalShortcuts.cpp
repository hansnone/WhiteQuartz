#include "GlobalShortcuts.h"
#include "../spotlight/SpotlightController.h"
#include "../menubar/MenuBarController.h"
#include "../dock/DockController.h"
#include "../launchpad/LaunchpadController.h"
#include "../missioncontrol/MissionControlController.h"
#include <QProcess>
#include <QDebug>

namespace Quartz::Shell {

GlobalShortcuts::GlobalShortcuts(SpotlightController *spotlight,
                               MenuBarController *menubar,
                               DockController *dock,
                               LaunchpadController *launchpad,
                               MissionControlController *missionControl,
                               QObject *parent)
    : QObject(parent)
    , m_spotlight(spotlight)
    , m_menubar(menubar)
    , m_dock(dock)
    , m_launchpad(launchpad)
    , m_missionControl(missionControl)
{
}

bool GlobalShortcuts::registerService()
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        return false;
    }

    if (!bus.registerObject(QStringLiteral("/org/quartz/GlobalShortcuts"), this,
                            QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qWarning() << "[GlobalShortcuts] Error al registrar objeto D-Bus:" << bus.lastError().message();
        return false;
    }

    bus.registerService(QStringLiteral("org.quartz.GlobalShortcuts"));
    return true;
}

void GlobalShortcuts::TriggerSpotlight()
{
    if (m_spotlight) {
        m_spotlight->toggle();
    }
}

void GlobalShortcuts::TriggerLaunchpad()
{
    if (m_launchpad) {
        m_launchpad->toggle();
    }
}

void GlobalShortcuts::TriggerMissionControl()
{
    if (m_missionControl) {
        m_missionControl->toggle();
    }

    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.quartz.Compositor"),
        QStringLiteral("/org/quartz/Compositor/StageManager"),
        QStringLiteral("org.quartz.Compositor.StageManager"),
        QStringLiteral("toggle_mission_control")
    );
    QDBusConnection::sessionBus().send(msg);
}

void GlobalShortcuts::TriggerAppExpose()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.quartz.Compositor"),
        QStringLiteral("/org/quartz/Compositor/StageManager"),
        QStringLiteral("org.quartz.Compositor.StageManager"),
        QStringLiteral("toggle_app_expose")
    );
    QDBusConnection::sessionBus().send(msg);
}

void GlobalShortcuts::TriggerLockScreen()
{
    if (m_menubar) {
        m_menubar->triggerAppleMenuAction(QStringLiteral("lock"));
    }
}

void GlobalShortcuts::TriggerForceQuit()
{
    if (m_menubar) {
        m_menubar->triggerAppleMenuAction(QStringLiteral("forcequit"));
    }
}

void GlobalShortcuts::TriggerScreenshot(int mode)
{
    Q_EMIT screenshotRequested(mode);

    if (mode == 1) {
        // Pantalla completa (Ctrl+Shift+3)
        QProcess::startDetached(QStringLiteral("spectacle"), { QStringLiteral("-b"), QStringLiteral("-f") });
    } else if (mode == 2) {
        // Área o región (Ctrl+Shift+4)
        QProcess::startDetached(QStringLiteral("spectacle"), { QStringLiteral("-r") });
    } else {
        // Utilidad interactiva (Ctrl+Shift+5)
        QProcess::startDetached(QStringLiteral("spectacle"), { QStringLiteral("-g") });
    }
}

} // namespace Quartz::Shell
