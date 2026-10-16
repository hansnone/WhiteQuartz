#pragma once

#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>

namespace Quartz::Shell {

class SpotlightController;
class MenuBarController;
class DockController;
class LaunchpadController;
class MissionControlController;

class GlobalShortcuts : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.quartz.GlobalShortcuts")

public:
    explicit GlobalShortcuts(SpotlightController *spotlight,
                            MenuBarController *menubar,
                            DockController *dock,
                            LaunchpadController *launchpad = nullptr,
                            MissionControlController *missionControl = nullptr,
                            QObject *parent = nullptr);

    bool registerService();

public Q_SLOTS:
    void TriggerSpotlight();
    void TriggerLaunchpad();
    void TriggerMissionControl();
    void TriggerAppExpose();
    void TriggerLockScreen();
    void TriggerForceQuit();
    void TriggerScreenshot(int mode); // 1 = full, 2 = rect, 3 = interactive

Q_SIGNALS:
    void screenshotRequested(int mode);

private:
    SpotlightController *m_spotlight;
    MenuBarController *m_menubar;
    DockController *m_dock;
    LaunchpadController *m_launchpad;
    MissionControlController *m_missionControl;
};

} // namespace Quartz::Shell
