#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "settingsmodel.h"
#include "../style/quartzstyle.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("QuartzSettings"));
    app.setOrganizationName(QStringLiteral("QuartzDE"));

    // Aplicar estilo Aqua nativo de QuartzDE
    QApplication::setStyle(new Quartz::Style::QuartzStyle);

    QQmlApplicationEngine engine;
    auto *navModel = new Quartz::Settings::SettingsNavigationModel(&app);
    engine.rootContext()->setContextProperty(QStringLiteral("settingsModel"), navModel);

    // Cargar interfaz principal de Ajustes
    const QUrl url(QStringLiteral("qrc:/qml/SettingsMainWindow.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    // Si no encuentra recurso qrc, cargar dinámicamente desde el path de desarrollo
    engine.load(url);
    if (engine.rootObjects().isEmpty()) {
        engine.load(QUrl::fromLocalFile(QStringLiteral(QUARTZ_SETTINGS_QML_DIR "/SettingsMainWindow.qml")));
    }

    return app.exec();
}
