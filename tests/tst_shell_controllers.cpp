#include <QtTest>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QFile>
#include "spotlight/SpotlightController.h"
#include "quicklook/QuickLookController.h"
#include "dock/DockController.h"
#include "menubar/MenuBarController.h"
#include "menubar/appmenuregistrar.h"
#include "notifications/NotificationServer.h"
#include "notifications/NotificationModel.h"
#include "controlcenter/SystemHardwareController.h"
#include "shortcuts/GlobalShortcuts.h"
#include "corners/HotCornersController.h"
#include "sni/StatusNotifierWatcher.h"
#include "launchpad/LaunchpadController.h"
#include "missioncontrol/MissionControlController.h"
#include "services/QuartzServices.h"

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

        ql.onPreviewRequested(
            QStringLiteral("file:///home/user/document.pdf"),
            QStringLiteral("document.pdf"),
            QStringLiteral("Documento PDF - 524288 bytes"),
            QStringLiteral("Pdf")
        );
        QCOMPARE(ql.previewType(), QStringLiteral("Pdf"));

        ql.onPreviewRequested(
            QStringLiteral("file:///home/user/song.mp3"),
            QStringLiteral("song.mp3"),
            QStringLiteral("Audio (MP3) - 3145728 bytes"),
            QStringLiteral("Audio")
        );
        QCOMPARE(ql.previewType(), QStringLiteral("Audio"));
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

    void testNotificationServerAndModel() {
        Quartz::Shell::NotificationServer server;
        QCOMPARE(server.GetCapabilities().contains(QStringLiteral("body")), true);

        QString vendor, version, spec;
        QString name = server.GetServerInformation(vendor, version, spec);
        QCOMPARE(name, QStringLiteral("Quartz Notification Server"));
        QCOMPARE(spec, QStringLiteral("1.2"));

        Quartz::Shell::NotificationModel model(&server);
        QCOMPARE(model.activeBanners().size(), 0);
        QCOMPARE(model.unreadCount(), 0);

        QSignalSpy spyAdded(&model, &Quartz::Shell::NotificationModel::activeBannersChanged);
        QSignalSpy spyClosed(&server, &Quartz::Shell::NotificationServer::NotificationClosed);

        // Enviar notificación con intento de inyección de script para comprobar saneamiento
        uint id = server.Notify(
            QStringLiteral("TestApp"),
            0,
            QStringLiteral("dialog-information"),
            QStringLiteral("Alerta del Sistema"),
            QStringLiteral("Texto seguro <script>alert('xss');</script> y contenido"),
            QStringList{QStringLiteral("default"), QStringLiteral("Abrir")},
            QVariantMap{},
            10000
        );

        QVERIFY(id > 0);
        QCOMPARE(spyAdded.count(), 1);
        QCOMPARE(model.activeBanners().size(), 1);
        QCOMPARE(model.history().size(), 1);
        QCOMPARE(model.unreadCount(), 1);

        QVariantMap notif = model.activeBanners().first().toMap();
        QCOMPARE(notif[QStringLiteral("appName")].toString(), QStringLiteral("TestApp"));
        QCOMPARE(notif[QStringLiteral("summary")].toString(), QStringLiteral("Alerta del Sistema"));
        // Comprobar saneamiento: el script ha debido ser eliminado
        QVERIFY(!notif[QStringLiteral("body")].toString().contains(QStringLiteral("<script>")));

        // Cerrar notificación
        model.dismiss(id);
        QCOMPARE(model.activeBanners().size(), 0);
        QCOMPARE(spyClosed.count(), 1);

        // Centro de notificaciones: abrirlo marca como leídas
        QCOMPARE(model.centerVisible(), false);
        model.toggleCenter();
        QCOMPARE(model.centerVisible(), true);
        QCOMPARE(model.unreadCount(), 0);

        model.clearAll();
        QCOMPARE(model.history().size(), 0);
    }

    void testSystemHardwareController() {
        Quartz::Shell::SystemHardwareController hw;

        QSignalSpy volSpy(&hw, &Quartz::Shell::SystemHardwareController::volumeChanged);
        QSignalSpy muteSpy(&hw, &Quartz::Shell::SystemHardwareController::isMutedChanged);
        QSignalSpy brightSpy(&hw, &Quartz::Shell::SystemHardwareController::brightnessChanged);
        QSignalSpy osdSpy(&hw, &Quartz::Shell::SystemHardwareController::osdTriggered);

        // Probar ajuste y límites de volumen
        hw.setVolume(50);
        QCOMPARE(hw.volume(), 50);
        QCOMPARE(volSpy.count(), 1);
        QVERIFY(osdSpy.count() >= 1);

        hw.setVolume(150); // Clamping a 100
        QCOMPARE(hw.volume(), 100);

        hw.setVolume(-20); // Clamping a 0 (y auto mute)
        QCOMPARE(hw.volume(), 0);
        QCOMPARE(hw.isMuted(), true);

        // Probar alternancia de mute
        hw.setIsMuted(false);
        QCOMPARE(hw.isMuted(), false);

        // Probar brillo y límites
        hw.setBrightness(45);
        QCOMPARE(hw.brightness(), 45);
        QCOMPARE(brightSpy.count(), 1);

        hw.setBrightness(200); // Clamping a 100
        QCOMPARE(hw.brightness(), 100);

        // Probar No Molestar
        QCOMPARE(hw.doNotDisturb(), false);
        hw.setDoNotDisturb(true);
        QCOMPARE(hw.doNotDisturb(), true);

        // Probar visibilidad de Centro de Control
        QCOMPARE(hw.visible(), false);
        hw.toggleControlCenter();
        QCOMPARE(hw.visible(), true);
    }

    void testGlobalShortcuts() {
        Quartz::Shell::SpotlightController spotlight;
        Quartz::Shell::MenuBarController menubar;
        Quartz::Shell::DockController dock;

        Quartz::Shell::GlobalShortcuts shortcuts(&spotlight, &menubar, &dock);
        QSignalSpy spyScreen(&shortcuts, &Quartz::Shell::GlobalShortcuts::screenshotRequested);

        // Probar trigger Spotlight
        QCOMPARE(spotlight.visible(), false);
        shortcuts.TriggerSpotlight();
        QCOMPARE(spotlight.visible(), true);

        // Probar captura de pantalla completa
        shortcuts.TriggerScreenshot(1);
        QCOMPARE(spyScreen.count(), 1);
        QCOMPARE(spyScreen.first().first().toInt(), 1);
    }

    void testHotCornersAndSNI() {
        // Probar HotCornersController
        Quartz::Shell::HotCornersController corners;
        QCOMPARE(corners.enabled(), true);
        corners.setEnabled(false);
        QCOMPARE(corners.enabled(), false);
        corners.setEnabled(true);
        QCOMPARE(corners.enabled(), true);

        // Probar StatusNotifierWatcher
        Quartz::Shell::StatusNotifierWatcher sni;
        QCOMPARE(sni.isHostRegistered(), true);
        QCOMPARE(sni.protocolVersion(), 0);
        QCOMPARE(sni.registeredItems().size(), 0);

        QSignalSpy spyItem(&sni, &Quartz::Shell::StatusNotifierWatcher::StatusNotifierItemRegistered);
        sni.RegisterStatusNotifierItem(QStringLiteral("org.kde.testitem/StatusNotifierItem"));
        QCOMPARE(spyItem.count(), 1);
        QCOMPARE(sni.registeredItems().size(), 1);
        QCOMPARE(sni.registeredItems().first(), QStringLiteral("org.kde.testitem/StatusNotifierItem"));

        // Probar dock contextual app actions (quitApp, forceQuitApp, toggleKeepInDock)
        Quartz::Shell::DockController dock;
        dock.quitApp(QStringLiteral("test-dummy-app"));
        dock.forceQuitApp(QStringLiteral("test-dummy-app"));
        dock.toggleKeepInDock(QStringLiteral("test-dummy-app"));
    }

    void testLaunchpadAndMissionControl() {
        // Probar LaunchpadController
        Quartz::Shell::LaunchpadController launchpad;
        QCOMPARE(launchpad.visible(), false);

        launchpad.toggle();
        QCOMPARE(launchpad.visible(), true);

        QVERIFY(launchpad.apps().size() > 0);
        QVERIFY(launchpad.pageCount() >= 1);

        launchpad.setFilterText(QStringLiteral("Finder"));
        QVERIFY(launchpad.apps().size() >= 1);
        QCOMPARE(launchpad.apps().first().toMap().value(QStringLiteral("name")).toString(), QStringLiteral("Finder"));

        launchpad.setVisible(false);
        QCOMPARE(launchpad.visible(), false);
        QCOMPARE(launchpad.filterText().isEmpty(), true);

        // Probar MissionControlController
        Quartz::Shell::MissionControlController mc;
        QCOMPARE(mc.active(), false);

        mc.toggle();
        QCOMPARE(mc.active(), true);

        int initialSpaces = mc.spaces().size();
        QVERIFY(initialSpaces >= 1);

        mc.addSpace();
        QCOMPARE(mc.spaces().size(), initialSpaces + 1);

        mc.switchToSpace(1);
        QCOMPARE(mc.currentSpaceIndex(), 1);
        QCOMPARE(mc.active(), false); // Cambiar de espacio oculta Mission Control

        mc.removeSpace(initialSpaces);
        QCOMPARE(mc.spaces().size(), initialSpaces);

        // Probar GlobalShortcuts con Launchpad y Mission Control
        Quartz::Shell::SpotlightController spotlight;
        Quartz::Shell::MenuBarController menubar;
        Quartz::Shell::DockController dock;
        Quartz::Shell::GlobalShortcuts shortcuts(&spotlight, &menubar, &dock, &launchpad, &mc);

        shortcuts.TriggerLaunchpad();
        QCOMPARE(launchpad.visible(), true);
        shortcuts.TriggerLaunchpad();
        QCOMPARE(launchpad.visible(), false);

        shortcuts.TriggerMissionControl();
        QCOMPARE(mc.active(), true);
        shortcuts.TriggerMissionControl();
        QCOMPARE(mc.active(), false);
    }

    void testQuartzSystemServices() {
        // 1. Time Machine
        Quartz::Shell::TimeMachineService tm;
        QCOMPARE(tm.status(), QStringLiteral("Inactivo"));
        QVERIFY(tm.snapshots().size() >= 1);

        tm.startBackup();
        QCOMPARE(tm.status(), QStringLiteral("Completado"));
        QCOMPARE(tm.progress(), 100);
        QVERIFY(!tm.lastBackupTime().isEmpty());
        QVERIFY(tm.snapshots().size() >= 2);

        // 2. Keychain (Llavero)
        Quartz::Shell::KeychainService kc;
        kc.storeSecret(QStringLiteral("com.quartz.test"), QStringLiteral("admin"), QStringLiteral("secret123"));
        QCOMPARE(kc.lookupSecret(QStringLiteral("com.quartz.test"), QStringLiteral("admin")), QStringLiteral("secret123"));
        QCOMPARE(kc.listKeys().size(), 1);
        QVERIFY(kc.deleteSecret(QStringLiteral("com.quartz.test"), QStringLiteral("admin")));
        QCOMPARE(kc.lookupSecret(QStringLiteral("com.quartz.test"), QStringLiteral("admin")), QString());

        // 3. PrintService (CUPS / Impresoras)
        Quartz::Shell::PrintService ps;
        QVERIFY(ps.printers().size() >= 1);
        QCOMPARE(ps.cancelJob(QString()), false);
        QCOMPARE(ps.cancelJob(QStringLiteral("job-101")), true);
    }
};

QTEST_MAIN(TestShellControllers)
#include "tst_shell_controllers.moc"
