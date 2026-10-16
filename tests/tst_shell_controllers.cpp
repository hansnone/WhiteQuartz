#include <QtTest>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QFile>
#include "spotlight/SpotlightController.h"
#include "quicklook/QuickLookController.h"
#include "dock/DockController.h"
#include "menubar/MenuBarController.h"
#include "menubar/appmenuregistrar.h"

class TestShellControllers : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testSpotlightVisibilityAndSearch() {
        Quartz::Shell::SpotlightController spotlight;
        QCOMPARE(spotlight.visible(), false);

        QSignalSpy visSpy(&spotlight, &Quartz::Shell::SpotlightController::visibleChanged);
        spotlight.toggle();
        QCOMPARE(spotlight.visible(), true);
        QCOMPARE(visSpy.count(), 1);

        spotlight.search(QStringLiteral("25+75"));
        QVERIFY(!spotlight.results().isEmpty());
        QVariantMap calcResult = spotlight.results().first().toMap();
        QCOMPARE(calcResult[QStringLiteral("title")].toString(), QStringLiteral("Cálculo"));
        QCOMPARE(calcResult[QStringLiteral("subtitle")].toString(), QStringLiteral("25+75"));
        QCOMPARE(calcResult[QStringLiteral("category")].toString(), QStringLiteral("Calculation"));
        QCOMPARE(calcResult[QStringLiteral("icon_name")].toString(), QStringLiteral("accessories-calculator"));

        spotlight.search(QStringLiteral("Terminal"));
        QVERIFY(!spotlight.results().isEmpty());
        QVariantMap appResult = spotlight.results().first().toMap();
        QCOMPARE(appResult[QStringLiteral("title")].toString(), QStringLiteral("Terminal"));
        QCOMPARE(appResult[QStringLiteral("payload")].toString(), QStringLiteral("x-terminal-emulator"));
        QCOMPARE(appResult[QStringLiteral("category")].toString(), QStringLiteral("App"));

        spotlight.setVisible(false);
        QCOMPARE(spotlight.visible(), false);
    }

    void testQuickLookPreviewHandling() {
        Quartz::Shell::QuickLookController ql;
        QCOMPARE(ql.visible(), false);

        QSignalSpy visSpy(&ql, &Quartz::Shell::QuickLookController::visibleChanged);
        QSignalSpy detailSpy(&ql, &Quartz::Shell::QuickLookController::fileDetailsChanged);

        // Crear fichero de prueba temporal para vista previa de texto
        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());
        tempFile.write("Hello QuartzDE\nQuick Look Preview Content\n");
        tempFile.flush();

        ql.onPreviewRequested(
            QUrl::fromLocalFile(tempFile.fileName()).toString(),
            QStringLiteral("test.txt"),
            QStringLiteral("Texto plano - 42 bytes"),
            QStringLiteral("Text"),
            QStringLiteral("Hello QuartzDE\nQuick Look Preview Content\n")
        );

        QCOMPARE(ql.visible(), true);
        QCOMPARE(visSpy.count(), 1);
        QCOMPARE(detailSpy.count(), 1);
        QCOMPARE(ql.fileName(), QStringLiteral("test.txt"));
        QCOMPARE(ql.previewType(), QStringLiteral("Text"));
        QVERIFY(ql.textContent().contains(QStringLiteral("Hello QuartzDE")));

        ql.closePreview();
        QCOMPARE(ql.visible(), false);
    }

    void testQuickLookImageHandling() {
        Quartz::Shell::QuickLookController ql;
        ql.onPreviewRequested(
            QStringLiteral("file:///usr/share/backgrounds/quartz.png"),
            QStringLiteral("quartz.png"),
            QStringLiteral("Imagen (PNG) - 102400 bytes"),
            QStringLiteral("Image")
        );
        QCOMPARE(ql.visible(), true);
        QCOMPARE(ql.fileName(), QStringLiteral("quartz.png"));
        QCOMPARE(ql.previewType(), QStringLiteral("Image"));
    }

    void testDockGaussianPhysicsAndBounce() {
        Quartz::Shell::DockController dock;

        // Magnificación Gaussiana continua
        qreal maxMagnified = dock.calculateGaussianMagnification(0.0, 65.0);
        QCOMPARE(maxMagnified, static_cast<qreal>(Quartz::StyleMetrics::DockMaxMagnifiedSize));

        qreal midMagnified = dock.calculateGaussianMagnification(65.0, 65.0);
        QVERIFY(midMagnified > Quartz::StyleMetrics::DockBaseIconSize);
        QVERIFY(midMagnified < Quartz::StyleMetrics::DockMaxMagnifiedSize);

        qreal distantMagnified = dock.calculateGaussianMagnification(500.0, 65.0);
        QCOMPARE(qRound(distantMagnified), Quartz::StyleMetrics::DockBaseIconSize);

        // Física parabólica de rebote: y(t) = -4 * h * t * (t - 1)
        QCOMPARE(dock.calculateParabolicBounce(0.0, 22.0), 0.0);
        QCOMPARE(dock.calculateParabolicBounce(0.5, 22.0), 22.0);
        QCOMPARE(dock.calculateParabolicBounce(1.0, 22.0), 0.0);
        QCOMPARE(dock.calculateParabolicBounce(-0.1, 22.0), 0.0);
        QCOMPARE(dock.calculateParabolicBounce(1.1, 22.0), 0.0);
    }

    void testDockCommandLineParsing() {
        Quartz::Shell::DockController dock;

        QVERIFY(dock.parseCommandLine(QString()).isEmpty());
        QVERIFY(dock.parseCommandLine(QStringLiteral("   ")).isEmpty());

        QStringList simple = dock.parseCommandLine(QStringLiteral("quartz-finder"));
        QCOMPARE(simple, QStringList{ QStringLiteral("quartz-finder") });

        QStringList withTilde = dock.parseCommandLine(QStringLiteral("quartz-finder ~/Descargas"));
        QCOMPARE(withTilde, QStringList({ QStringLiteral("quartz-finder"), QDir::homePath() + QStringLiteral("/Descargas") }));

        QStringList withQuotes = dock.parseCommandLine(QStringLiteral("sh -c \"echo hello\""));
        QCOMPARE(withQuotes, QStringList({ QStringLiteral("sh"), QStringLiteral("-c"), QStringLiteral("echo hello") }));
    }

    void testDockAppAttentionSignal() {
        Quartz::Shell::DockController dock;
        QSignalSpy spy(&dock, &Quartz::Shell::DockController::appBouncingRequested);

        dock.requestAppAttention(QStringLiteral("quartz-finder"));
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().at(0).toString(), QStringLiteral("quartz-finder"));
    }

    void testAppMenuRegistrar() {
        Quartz::Shell::AppMenuRegistrar registrar;
        registrar.registerService();

        QSignalSpy regSpy(&registrar, &Quartz::Shell::AppMenuRegistrar::menuRegistered);
        QSignalSpy unregSpy(&registrar, &Quartz::Shell::AppMenuRegistrar::menuUnregistered);

        uint winId = 4242;
        QDBusObjectPath menuPath(QStringLiteral("/MenuBar/4242"));
        registrar.RegisterWindow(winId, menuPath);

        QCOMPARE(regSpy.count(), 1);
        QCOMPARE(regSpy.first().at(0).toUInt(), winId);
        QCOMPARE(regSpy.first().at(2).toString(), QStringLiteral("/MenuBar/4242"));

        QDBusObjectPath fetchedPath;
        QString service = registrar.GetMenuForWindow(winId, fetchedPath);
        QCOMPARE(fetchedPath.path(), QStringLiteral("/MenuBar/4242"));

        registrar.UnregisterWindow(winId);
        QCOMPARE(unregSpy.count(), 1);
        QCOMPARE(unregSpy.first().at(0).toUInt(), winId);

        QDBusObjectPath emptyPath;
        registrar.GetMenuForWindow(winId, emptyPath);
        QVERIFY(emptyPath.path().isEmpty());
    }

    void testMenuBarController() {
        Quartz::Shell::MenuBarController menuBar;

        // Título de la app activa por defecto
        QCOMPARE(menuBar.activeAppTitle(), QStringLiteral("Finder"));

        // Menús macOS por defecto
        QVariantList defaultMenus = menuBar.activeAppMenus();
        QCOMPARE(defaultMenus.size(), 5);

        QStringList expectedLabels = {
            QStringLiteral("Archivo"),
            QStringLiteral("Edición"),
            QStringLiteral("Visualización"),
            QStringLiteral("Ventana"),
            QStringLiteral("Ayuda")
        };

        for (int i = 0; i < defaultMenus.size(); ++i) {
            QVariantMap menuMap = defaultMenus.at(i).toMap();
            QCOMPARE(menuMap.value(QStringLiteral("label")).toString(), expectedLabels.at(i));
            QVERIFY(!menuMap.value(QStringLiteral("children")).toList().isEmpty());
        }

        // Cambio de ventana activa
        menuBar.onActiveWindowChanged(QStringLiteral("org.kde.kate"), QStringLiteral("Kate"));
        QCOMPARE(menuBar.activeAppTitle(), QStringLiteral("Kate"));

        // Invocación segura de acción de menú
        menuBar.triggerMenuAction(101);
    }

    void testDockStacksAndTrashState() {
        Quartz::Shell::DockController dock;

        // Directorio temporal para simular ~/Descargas
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        // Crear subcarpeta
        QDir dir(tempDir.path());
        QVERIFY(dir.mkdir(QStringLiteral("Subcarpeta")));

        // Crear archivos simulados con diferentes extensiones
        QFile imgFile(tempDir.path() + QStringLiteral("/foto.png"));
        QVERIFY(imgFile.open(QIODevice::WriteOnly));
        imgFile.write("dummy-png-data");
        imgFile.close();

        QFile pdfFile(tempDir.path() + QStringLiteral("/manual.pdf"));
        QVERIFY(pdfFile.open(QIODevice::WriteOnly));
        pdfFile.write("dummy-pdf-data");
        pdfFile.close();

        QVariantList stackItems = dock.listStackDirectory(tempDir.path());
        QCOMPARE(stackItems.size(), 3);

        bool foundDir = false;
        bool foundImg = false;
        bool foundPdf = false;

        for (const QVariant &v : stackItems) {
            QVariantMap item = v.toMap();
            QString name = item.value(QStringLiteral("name")).toString();
            if (name == QStringLiteral("Subcarpeta")) {
                foundDir = true;
                QCOMPARE(item.value(QStringLiteral("isDir")).toBool(), true);
                QCOMPARE(item.value(QStringLiteral("icon")).toString(), QStringLiteral("folder"));
            } else if (name == QStringLiteral("foto.png")) {
                foundImg = true;
                QCOMPARE(item.value(QStringLiteral("icon")).toString(), QStringLiteral("image-x-generic"));
            } else if (name == QStringLiteral("manual.pdf")) {
                foundPdf = true;
                QCOMPARE(item.value(QStringLiteral("icon")).toString(), QStringLiteral("application-pdf"));
            }
        }

        QVERIFY(foundDir);
        QVERIFY(foundImg);
        QVERIFY(foundPdf);

        // Estado de papelera
        QString trash = dock.trashIcon();
        QVERIFY(trash == QStringLiteral("user-trash") || trash == QStringLiteral("user-trash-full"));
        dock.updateTrashState();
    }

    void testDockDynamicWindowTracking() {
        Quartz::Shell::DockController dock;

        // Lista inicial de apps
        QVariantList initialApps = dock.apps();
        QVERIFY(!initialApps.isEmpty());

        // Comprobar que Finder está inicialmente en estado no ejecutándose
        bool finderInitialRunning = false;
        for (const QVariant &v : initialApps) {
            QVariantMap m = v.toMap();
            if (m.value(QStringLiteral("cmd")).toString() == QStringLiteral("quartz-finder")) {
                finderInitialRunning = m.value(QStringLiteral("running")).toBool();
            }
        }
        QCOMPARE(finderInitialRunning, false);

        // Añadir ventana de aplicación no anclada (Firefox)
        dock.onWindowAdded(QStringLiteral("firefox"), QStringLiteral("Firefox"), QStringLiteral("firefox"), true);
        QVariantList updatedApps = dock.apps();

        bool foundFirefox = false;
        int firefoxIndex = -1;
        int separatorIndex = -1;

        for (int i = 0; i < updatedApps.size(); ++i) {
            QVariantMap m = updatedApps.at(i).toMap();
            if (m.value(QStringLiteral("cmd")).toString() == QStringLiteral("firefox")) {
                foundFirefox = true;
                firefoxIndex = i;
                QCOMPARE(m.value(QStringLiteral("running")).toBool(), true);
            }
            if (m.value(QStringLiteral("isSeparator")).toBool()) {
                separatorIndex = i;
            }
        }

        QVERIFY(foundFirefox);
        // Debe aparecer antes del separador
        QVERIFY(firefoxIndex < separatorIndex);

        // Al cerrarse la ventana de Firefox, desaparece del Dock
        dock.onWindowRemoved(QStringLiteral("firefox"));
        QVariantList appsAfterClose = dock.apps();
        bool stillHasFirefox = false;
        for (const QVariant &v : appsAfterClose) {
            if (v.toMap().value(QStringLiteral("cmd")).toString() == QStringLiteral("firefox")) {
                stillHasFirefox = true;
            }
        }
        QCOMPARE(stillHasFirefox, false);

        // Al abrirse ventana de Finder (anclado), cambia 'running' a true
        dock.onWindowAdded(QStringLiteral("quartz-finder"), QStringLiteral("Finder"), QStringLiteral("system-file-manager"), true);
        bool finderNowRunning = false;
        for (const QVariant &v : dock.apps()) {
            QVariantMap m = v.toMap();
            if (m.value(QStringLiteral("cmd")).toString() == QStringLiteral("quartz-finder")) {
                finderNowRunning = m.value(QStringLiteral("running")).toBool();
            }
        }
        QCOMPARE(finderNowRunning, true);
    }
};

QTEST_MAIN(TestShellControllers)
#include "tst_shell_controllers.moc"
