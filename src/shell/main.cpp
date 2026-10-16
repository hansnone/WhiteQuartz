#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickWindow>
#include <QFile>
#include <QDebug>
#include "menubar/MenuBarController.h"
#include "dock/DockController.h"
#include "spotlight/SpotlightController.h"
#include "quicklook/QuickLookController.h"
#include "notifications/NotificationServer.h"
#include "notifications/NotificationModel.h"
#include "controlcenter/SystemHardwareController.h"
#include "shortcuts/GlobalShortcuts.h"
#include "corners/HotCornersController.h"
#include "sni/StatusNotifierWatcher.h"
#include "launchpad/LaunchpadController.h"
#include "missioncontrol/MissionControlController.h"
#include "services/QuartzServices.h"
#include "../style/quartzstyle.h"

#ifdef HAVE_LAYERSHELLQT
#include <LayerShellQt/Window>
#endif

#ifdef HAVE_KF6WINDOWSYSTEM
#include <KWindowEffects>
#endif

#ifdef HAVE_LAYERSHELLQT
static void configureLayerShell(QObject *rootObj, const QString &type) {
    auto *qwindow = qobject_cast<QQuickWindow*>(rootObj);
    if (!qwindow) {
        return;
    }
    auto *lWindow = LayerShellQt::Window::get(qwindow);
    if (!lWindow) {
        return;
    }

    if (type == QStringLiteral("menubar")) {
        lWindow->setLayer(LayerShellQt::Window::LayerTop);
        lWindow->setAnchors(LayerShellQt::Window::Anchors(LayerShellQt::Window::AnchorTop) | LayerShellQt::Window::AnchorLeft | LayerShellQt::Window::AnchorRight);
        lWindow->setExclusiveZone(24);
        lWindow->setScope(QStringLiteral("menubar"));
    } else if (type == QStringLiteral("dock")) {
        lWindow->setLayer(LayerShellQt::Window::LayerTop);
        lWindow->setAnchors(LayerShellQt::Window::Anchors(LayerShellQt::Window::AnchorBottom));
        lWindow->setExclusiveZone(0);
        lWindow->setScope(QStringLiteral("dock"));
    } else if (type == QStringLiteral("spotlight")) {
        lWindow->setLayer(LayerShellQt::Window::LayerOverlay);
        lWindow->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityOnDemand);
        lWindow->setScope(QStringLiteral("spotlight"));
    } else if (type == QStringLiteral("quicklook")) {
        lWindow->setLayer(LayerShellQt::Window::LayerOverlay);
        lWindow->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityOnDemand);
        lWindow->setScope(QStringLiteral("quicklook"));
    } else if (type == QStringLiteral("wallpaper")) {
        lWindow->setLayer(LayerShellQt::Window::LayerBackground);
        lWindow->setAnchors(LayerShellQt::Window::Anchors(LayerShellQt::Window::AnchorTop) | LayerShellQt::Window::AnchorBottom | LayerShellQt::Window::AnchorLeft | LayerShellQt::Window::AnchorRight);
        lWindow->setExclusiveZone(-1);
        lWindow->setScope(QStringLiteral("wallpaper"));
    } else if (type == QStringLiteral("notification-banner") || type == QStringLiteral("controlcenter") || type == QStringLiteral("notification-center") || type == QStringLiteral("osd")) {
        lWindow->setLayer(LayerShellQt::Window::LayerOverlay);
        lWindow->setScope(type);
    }
}
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("QuartzShell"));
    app.setOrganizationName(QStringLiteral("QuartzDE"));

    QApplication::setStyle(new Quartz::Style::QuartzStyle);

    auto *menuBarCtrl = new Quartz::Shell::MenuBarController(&app);
    auto *dockCtrl = new Quartz::Shell::DockController(&app);
    auto *spotlightCtrl = new Quartz::Shell::SpotlightController(&app);
    auto *quickLookCtrl = new Quartz::Shell::QuickLookController(&app);
    auto *notifServer = new Quartz::Shell::NotificationServer(&app);
    notifServer->registerService();
    auto *notifModel = new Quartz::Shell::NotificationModel(notifServer, &app);
    auto *hwCtrl = new Quartz::Shell::SystemHardwareController(&app);
    auto *launchpadCtrl = new Quartz::Shell::LaunchpadController(&app);
    auto *missionControlCtrl = new Quartz::Shell::MissionControlController(&app);
    auto *shortcuts = new Quartz::Shell::GlobalShortcuts(spotlightCtrl, menuBarCtrl, dockCtrl, launchpadCtrl, missionControlCtrl, &app);
    shortcuts->registerService();
    auto *sniWatcher = new Quartz::Shell::StatusNotifierWatcher(&app);
    sniWatcher->registerService();
    auto *timeMachine = new Quartz::Shell::TimeMachineService(&app);
    timeMachine->registerService();
    auto *keychain = new Quartz::Shell::KeychainService(&app);
    keychain->registerService();
    auto *printService = new Quartz::Shell::PrintService(&app);
    printService->registerService();
    auto *hotCorners = new Quartz::Shell::HotCornersController(&app);
    QObject::connect(hotCorners, &Quartz::Shell::HotCornersController::cornerTriggered, &app, [shortcuts, notifModel](int corner) {
        if (corner == 0) shortcuts->TriggerMissionControl();
        else if (corner == 1) notifModel->toggleCenter();
        else if (corner == 2) shortcuts->TriggerSpotlight();
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("menuBarCtrl"), menuBarCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("dockCtrl"), dockCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("spotlightCtrl"), spotlightCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("launchpadCtrl"), launchpadCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("missionControlCtrl"), missionControlCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("quickLookCtrl"), quickLookCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("notificationModel"), notifModel);
    engine.rootContext()->setContextProperty(QStringLiteral("hardwareCtrl"), hwCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("hotCornersCtrl"), hotCorners);
    engine.rootContext()->setContextProperty(QStringLiteral("timeMachineService"), timeMachine);
    engine.rootContext()->setContextProperty(QStringLiteral("keychainService"), keychain);
    engine.rootContext()->setContextProperty(QStringLiteral("printService"), printService);

    auto loadComponent = [&](const QString &qmlName, const QString &type) -> QObject* {
        QUrl url(QStringLiteral("qrc:/qml/") + qmlName);
        QQmlComponent component(&engine, url);
        if (component.isError()) {
#ifdef QUARTZ_SHELL_INSTALL_QML_DIR
            const QString installPath = QStringLiteral(QUARTZ_SHELL_INSTALL_QML_DIR "/") + qmlName;
            if (QFile::exists(installPath)) {
                component.loadUrl(QUrl::fromLocalFile(installPath));
            }
#endif
        }
        if (component.isError()) {
#ifdef QUARTZ_SHELL_QML_DIR
            const QString srcPath = QStringLiteral(QUARTZ_SHELL_QML_DIR "/") + qmlName;
            if (QFile::exists(srcPath)) {
                component.loadUrl(QUrl::fromLocalFile(srcPath));
            }
#endif
        }
        QObject *obj = component.create();
        if (obj) {
            auto *qwin = qobject_cast<QQuickWindow*>(obj);
            if (qwin) {
#ifdef HAVE_KF6WINDOWSYSTEM
                if (type != QStringLiteral("wallpaper")) {
                    KWindowEffects::enableBlurBehind(qwin, true);
                }
#endif
            }
#ifdef HAVE_LAYERSHELLQT
            configureLayerShell(obj, type);
#else
            Q_UNUSED(type);
#endif
            return obj;
        } else {
            qWarning() << "No se pudo crear componente QML:" << qmlName << component.errorString();
            return nullptr;
        }
    };

    QObject *wallpaper = loadComponent(QStringLiteral("WallpaperWindow.qml"), QStringLiteral("wallpaper"));
    QObject *menuBar = loadComponent(QStringLiteral("MenuBarWindow.qml"), QStringLiteral("menubar"));
    QObject *dock = loadComponent(QStringLiteral("DockWindow.qml"), QStringLiteral("dock"));
    QObject *spotlight = loadComponent(QStringLiteral("SpotlightOverlay.qml"), QStringLiteral("spotlight"));
    QObject *quicklook = loadComponent(QStringLiteral("QuickLookModal.qml"), QStringLiteral("quicklook"));
    loadComponent(QStringLiteral("NotificationBanner.qml"), QStringLiteral("notification-banner"));
    loadComponent(QStringLiteral("NotificationCenter.qml"), QStringLiteral("notification-center"));
    loadComponent(QStringLiteral("ControlCenterPopup.qml"), QStringLiteral("controlcenter"));
    loadComponent(QStringLiteral("OsdWindow.qml"), QStringLiteral("osd"));
    loadComponent(QStringLiteral("AppSwitcherOverlay.qml"), QStringLiteral("app-switcher"));
    loadComponent(QStringLiteral("ForceQuitModal.qml"), QStringLiteral("force-quit"));
    loadComponent(QStringLiteral("LaunchpadOverlay.qml"), QStringLiteral("launchpad"));
    loadComponent(QStringLiteral("MissionControlOverlay.qml"), QStringLiteral("mission-control"));

    // Fallback: si no cargaron los componentes modulares, cargar ShellWindow.qml
    if (!wallpaper && !menuBar && !dock && !spotlight && !quicklook) {
        qWarning() << "Cargando ShellWindow.qml como interfaz de respaldo...";
        engine.load(QUrl(QStringLiteral("qrc:/qml/ShellWindow.qml")));
    }

    return app.exec();
}
