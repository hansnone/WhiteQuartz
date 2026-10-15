#include <QtTest>
#include "settingsconfigcontroller.h"

class TestSettingsConfig : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testDefaults() {
        Quartz::Settings::SettingsConfigController ctrl;
        QCOMPARE(ctrl.stageManagerEnabled(), false);
        QCOMPARE(ctrl.appearanceMode(), QStringLiteral("Auto"));
        QCOMPARE(ctrl.themeName(), QStringLiteral("Aqua"));
        QCOMPARE(ctrl.iconTheme(), QStringLiteral("Colloid"));
    }

    void testSignalsAndMutations() {
        Quartz::Settings::SettingsConfigController ctrl;
        QSignalSpy smSpy(&ctrl, &Quartz::Settings::SettingsConfigController::stageManagerEnabledChanged);
        QSignalSpy appSpy(&ctrl, &Quartz::Settings::SettingsConfigController::appearanceModeChanged);
        QSignalSpy themeSpy(&ctrl, &Quartz::Settings::SettingsConfigController::themeNameChanged);

        ctrl.setStageManagerEnabled(true);
        QCOMPARE(smSpy.count(), 1);
        QCOMPARE(ctrl.stageManagerEnabled(), true);

        ctrl.setAppearanceMode(QStringLiteral("Dark"));
        QCOMPARE(appSpy.count(), 1);
        QCOMPARE(ctrl.appearanceMode(), QStringLiteral("Dark"));

        ctrl.setThemeName(QStringLiteral("Orchis-Dark"));
        QCOMPARE(themeSpy.count(), 1);
        QCOMPARE(ctrl.themeName(), QStringLiteral("Orchis-Dark"));
    }

    void testOnConfigChangedParsing() {
        Quartz::Settings::SettingsConfigController ctrl;
        QSignalSpy smSpy(&ctrl, &Quartz::Settings::SettingsConfigController::stageManagerEnabledChanged);
        QSignalSpy themeSpy(&ctrl, &Quartz::Settings::SettingsConfigController::themeNameChanged);

        QString testJson = QStringLiteral(R"({
            "stage_manager": { "enabled": true },
            "appearance": "Light",
            "theme_name": "Orchis",
            "icon_theme": "Colloid-dark"
        })");

        ctrl.onConfigChanged(testJson);
        QCOMPARE(ctrl.stageManagerEnabled(), true);
        QCOMPARE(ctrl.appearanceMode(), QStringLiteral("Light"));
        QCOMPARE(ctrl.themeName(), QStringLiteral("Orchis"));
        QCOMPARE(ctrl.iconTheme(), QStringLiteral("Colloid-dark"));
    }
};

QTEST_MAIN(TestSettingsConfig)
#include "tst_settingsconfig.moc"
