#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "menubar/MenuBarController.h"
#include "dock/DockController.h"
#include "../style/quartzstyle.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("QuartzShell"));
    app.setOrganizationName(QStringLiteral("QuartzDE"));

    QApplication::setStyle(new Quartz::Style::QuartzStyle);

    auto *menuBarCtrl = new Quartz::Shell::MenuBarController(&app);
    auto *dockCtrl = new Quartz::Shell::DockController(&app);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("menuBarCtrl"), menuBarCtrl);
    engine.rootContext()->setContextProperty(QStringLiteral("dockCtrl"), dockCtrl);

    const QUrl url(QStringLiteral("qrc:/qml/ShellWindow.qml"));
    engine.load(url);
    if (engine.rootObjects().isEmpty()) {
        engine.load(QUrl::fromLocalFile(QStringLiteral(QUARTZ_SHELL_QML_DIR "/ShellWindow.qml")));
    }

    return app.exec();
}
