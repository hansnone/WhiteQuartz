#include <QTest>
#include "../src/settings/settingsmodel.h"

class TestSettingsModel : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testInitialPopulation() {
        Quartz::Settings::SettingsNavigationModel model;
        QCOMPARE(model.totalItemsCount(), 18);
        QCOMPARE(model.rowCount(), 18);
    }

    void testFiltering() {
        Quartz::Settings::SettingsNavigationModel model;

        // Búsqueda por título
        model.setFilterQuery(QStringLiteral("Wi-Fi"));
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.firstMatchingPaneId(), QStringLiteral("wifi"));

        // Búsqueda por palabra clave ("oscuro" -> Apariencia)
        model.setFilterQuery(QStringLiteral("oscuro"));
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.firstMatchingPaneId(), QStringLiteral("appearance"));

        // Limpiar filtro
        model.setFilterQuery(QString());
        QCOMPARE(model.rowCount(), 18);
    }
};

QTEST_MAIN(TestSettingsModel)
#include "tst_settings_model.moc"
