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
#include "../style/quartzstyle.h"

#ifdef HAVE_LAYERSHELLQT
#include <LayerShellQt/Window>
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

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("menuBarCtrl"), menuBarCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("dockCtrl"), dockCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("spotlightCtrl"), spotlightCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("quickLookCtrl"), quickLookCtrl);

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

    // Fallback: si no cargaron los componentes modulares, cargar ShellWindow.qml
    if (!wallpaper && !menuBar && !dock && !spotlight && !quicklook) {
        qWarning() << "Cargando ShellWindow.qml como interfaz de respaldo...";
        engine.load(QUrl(QStringLiteral("qrc:/qml/ShellWindow.qml")));
    }

    return app.exec();
}
