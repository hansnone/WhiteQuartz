#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFile>
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

    const QUrl qrcUrl(QStringLiteral("qrc:/qml/ShellWindow.qml"));
    engine.load(qrcUrl);

    if (engine.rootObjects().isEmpty()) {
#ifdef QUARTZ_SHELL_INSTALL_QML_DIR
        const QString installPath = QStringLiteral(QUARTZ_SHELL_INSTALL_QML_DIR "/ShellWindow.qml");
        if (QFile::exists(installPath)) {
            engine.load(QUrl::fromLocalFile(installPath));
        }
#endif
    }

    if (engine.rootObjects().isEmpty()) {
#ifdef QUARTZ_SHELL_QML_DIR
        const QString sourcePath = QStringLiteral(QUARTZ_SHELL_QML_DIR "/ShellWindow.qml");
        if (QFile::exists(sourcePath)) {
            engine.load(QUrl::fromLocalFile(sourcePath));
        }
#endif
    }

    if (engine.rootObjects().isEmpty()) {
        qCritical("No se pudo cargar ShellWindow.qml desde recursos embebidos ni rutas de sistema.");
        return 1;
    }

    return app.exec();
}
