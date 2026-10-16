#include <QTest>
#include "../src/compositor/stagemanagereffect.h"
#include "../src/compositor/quartzdecoration.h"

class TestStageManagerMath : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testStateToggle() {
        Quartz::Compositor::StageManagerEffect effect;
        QCOMPARE(effect.isEnabled(), false);

        effect.setEnabled(true);
        QCOMPARE(effect.isEnabled(), true);
        QCOMPARE(static_cast<int>(effect.spatialMode()), static_cast<int>(Quartz::Compositor::SpatialMode::StageManager));
    }

    void testSpatialModes() {
        Quartz::Compositor::StageManagerEffect effect;

        // Toggle Mission Control (Super+Up)
        effect.toggleMissionControl();
        QCOMPARE(static_cast<int>(effect.spatialMode()), static_cast<int>(Quartz::Compositor::SpatialMode::MissionControl));

        // Toggle App Exposé (Super+Down)
        effect.toggleAppExpose(QStringLiteral("quartz-finder"));
        QCOMPARE(static_cast<int>(effect.spatialMode()), static_cast<int>(Quartz::Compositor::SpatialMode::AppExpose));
    }

    void testSquircleDistanceFormula() {
        qreal radius = 11.0;
        qreal power = 4.5;

        // Centro (0, 0)
        qreal distCenter = Quartz::Compositor::StageManagerEffect::calculateSquircleDistance(0, 0, radius, power);
        QCOMPARE(distCenter, 0.0);

        // Borde en eje X (11, 0) -> distancia debe ser exactamente 1.0
        qreal distBorderX = Quartz::Compositor::StageManagerEffect::calculateSquircleDistance(11.0, 0.0, radius, power);
        QVERIFY(qAbs(distBorderX - 1.0) < 0.001);

        // Borde en eje Y (0, 11) -> distancia debe ser exactamente 1.0
        qreal distBorderY = Quartz::Compositor::StageManagerEffect::calculateSquircleDistance(0.0, 11.0, radius, power);
        QVERIFY(qAbs(distBorderY - 1.0) < 0.001);

        // Punto exterior (12, 12) -> distancia > 1.0
        qreal distOutside = Quartz::Compositor::StageManagerEffect::calculateSquircleDistance(12.0, 12.0, radius, power);
        QVERIFY(distOutside > 1.0);
    }

    void testDropShadowKeyWindowVsInactive() {
        auto keyShadow = Quartz::Compositor::StageManagerEffect::shadowForWindow(true);
        auto inactiveShadow = Quartz::Compositor::StageManagerEffect::shadowForWindow(false);

        // La ventana activa debe tener mayor radio de difusión y opacidad
        QVERIFY(keyShadow.blurRadius > inactiveShadow.blurRadius);
        QVERIFY(keyShadow.opacity > inactiveShadow.opacity);
        QCOMPARE(keyShadow.blurRadius, 32.0);
        QCOMPARE(inactiveShadow.blurRadius, 14.0);
    }

    void testTransformMatrix() {
        Quartz::Compositor::StageManagerEffect effect;
        effect.setEnabled(true);

        QRectF winGeo(100, 100, 800, 600);
        QMatrix4x4 mat = effect.calculateStageTransform(winGeo, 0);

        QVERIFY(!mat.isIdentity());
        bool invertible = false;
        mat.inverted(&invertible);
        QVERIFY(invertible);
    }

    void testQuartzDecorationMetrics() {
        Quartz::Compositor::QuartzDecoration deco;
        QCOMPARE(deco.titleBarHeight(), 28);

        deco.setUnifiedToolbar(true);
        QCOMPARE(deco.titleBarHeight(), 52);

        QRect closeR = deco.closeButtonRect();
        QRect minR = deco.minimizeButtonRect();
        QRect maxR = deco.maximizeButtonRect();

        QCOMPARE(closeR.left(), 14);
        QCOMPARE(closeR.width(), 12);
        QCOMPARE(closeR.height(), 12);

        QCOMPARE(minR.left(), 34);
        QCOMPARE(maxR.left(), 54);
    }

    void testQuartzDecorationHoverAndTiling() {
        Quartz::Compositor::QuartzDecoration deco;
        QCOMPARE(static_cast<int>(deco.hoverState()), static_cast<int>(Quartz::Compositor::TrafficLightHoverState::None));

        deco.setHoverState(Quartz::Compositor::TrafficLightHoverState::CloseHovered);
        QCOMPARE(static_cast<int>(deco.hoverState()), static_cast<int>(Quartz::Compositor::TrafficLightHoverState::CloseHovered));

        QCOMPARE(deco.documentHasUnsavedChanges(), false);
        deco.setDocumentHasUnsavedChanges(true);
        QCOMPARE(deco.documentHasUnsavedChanges(), true);

        deco.onZoomHoverEntered(QPoint(50, 10));
        QCOMPARE(static_cast<int>(deco.hoverState()), static_cast<int>(Quartz::Compositor::TrafficLightHoverState::ZoomHovered));

        deco.onZoomHoverLeft();
        QCOMPARE(static_cast<int>(deco.hoverState()), static_cast<int>(Quartz::Compositor::TrafficLightHoverState::None));
        QCOMPARE(deco.isTilingMenuVisible(), false);
    }

    void testShadowImageGeneration() {
        QImage activeShadow = Quartz::Compositor::QuartzDecoration::generateShadowImage(200, 150, true);
        QImage inactiveShadow = Quartz::Compositor::QuartzDecoration::generateShadowImage(200, 150, false);

        QVERIFY(!activeShadow.isNull());
        QVERIFY(!inactiveShadow.isNull());
        QVERIFY(activeShadow.width() >= 200);
        QVERIFY(activeShadow.height() >= 150);
    }

    void testStageManagerDBusMethods() {
        Quartz::Compositor::StageManagerEffect effect;
        QCOMPARE(effect.isEnabledSlot(), false);

        effect.addWindowToCurrentStage(QStringLiteral("win-101"));
        QCOMPARE(effect.activeStageCount(), 1);

        effect.activateStageGroup(QStringLiteral("group_1"));
        effect.setEnabled(true);
        QCOMPARE(effect.isEnabledSlot(), true);
    }

    void testMissionControlLayout() {
        Quartz::Compositor::StageManagerEffect effect;
        QRectF screen(0, 0, 1920, 1080);
        QRectF winGeo(100, 100, 800, 600);

        int totalWindows = 4;
        QVector<QRectF> targetRects;

        for (int i = 0; i < totalWindows; ++i) {
            QMatrix4x4 mat = effect.calculateMissionControlTransform(winGeo, i, totalWindows, screen);
            QVERIFY(!mat.isIdentity());

            bool invertible = false;
            mat.inverted(&invertible);
            QVERIFY(invertible);

            // Mapear el rectángulo de la ventana según la matriz
            QPointF topLeft = mat.map(winGeo.topLeft());
            QPointF bottomRight = mat.map(winGeo.bottomRight());
            QRectF mapped(topLeft, bottomRight);

            // Debe estar completamente dentro de la pantalla
            QVERIFY(screen.contains(mapped));
            targetRects.append(mapped);
        }

        // Ninguna ventana debe solapar con otra
        for (int i = 0; i < totalWindows; ++i) {
            for (int j = i + 1; j < totalWindows; ++j) {
                QVERIFY(!targetRects[i].intersects(targetRects[j]));
            }
        }
    }

    void testAppExposeLayout() {
        Quartz::Compositor::StageManagerEffect effect;
        QRectF screen(0, 0, 1920, 1080);
        QRectF winGeo(50, 50, 1024, 768);

        int totalWindows = 3;
        QVector<QRectF> targetRects;

        for (int i = 0; i < totalWindows; ++i) {
            QMatrix4x4 mat = effect.calculateAppExposeTransform(winGeo, i, totalWindows, screen);
            QVERIFY(!mat.isIdentity());

            QPointF topLeft = mat.map(winGeo.topLeft());
            QPointF bottomRight = mat.map(winGeo.bottomRight());
            QRectF mapped(topLeft, bottomRight);

            QVERIFY(screen.contains(mapped));
            targetRects.append(mapped);
        }

        for (int i = 0; i < totalWindows; ++i) {
            for (int j = i + 1; j < totalWindows; ++j) {
                QVERIFY(!targetRects[i].intersects(targetRects[j]));
            }
        }
    }
};


QTEST_MAIN(TestStageManagerMath)
#include "tst_stagemanager_math.moc"
