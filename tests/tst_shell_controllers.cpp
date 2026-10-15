#include <QtTest>
#include "spotlight/SpotlightController.h"
#include "quicklook/QuickLookController.h"
#include "dock/DockController.h"

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

        spotlight.search(QStringLiteral("Terminal"));
        QVERIFY(!spotlight.results().isEmpty());
        QVariantMap appResult = spotlight.results().first().toMap();
        QCOMPARE(appResult[QStringLiteral("title")].toString(), QStringLiteral("Terminal"));
        QCOMPARE(appResult[QStringLiteral("payload")].toString(), QStringLiteral("x-terminal-emulator"));

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
            QStringLiteral("Text")
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
};

QTEST_MAIN(TestShellControllers)
#include "tst_shell_controllers.moc"
