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
};


QTEST_MAIN(TestStageManagerMath)
#include "tst_stagemanager_math.moc"
