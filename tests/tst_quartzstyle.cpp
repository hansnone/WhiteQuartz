#include <QTest>
#include <QApplication>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include "../src/style/quartzstyle.h"
#include "../src/style/quartzmetrics.h"

class TestQuartzStyle : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void initTestCase() {
        m_style = new Quartz::Style::QuartzStyle();
    }

    void cleanupTestCase() {
        delete m_style;
    }

    void testMetrics() {
        QCOMPARE(m_style->pixelMetric(QStyle::PM_TitleBarHeight), Quartz::StyleMetrics::StandardTitleBarHeight);
        QCOMPARE(Quartz::StyleMetrics::MenuBarHeight, 24);
        QCOMPARE(Quartz::StyleMetrics::DockBaseIconSize, 48);
        QCOMPARE(Quartz::StyleMetrics::DockMaxMagnifiedSize, 96);
        QCOMPARE(Quartz::StyleMetrics::TrafficLightButtonSize, 12);
        QCOMPARE(Quartz::StyleMetrics::TrafficLightSpacing, 8);
        QCOMPARE(Quartz::StyleMetrics::TrafficLightLeftMargin, 14);
    }

    void testTrafficLightSymbols() {
        QPixmap pix(20, 20);
        QPainter p(&pix);
        QRect r(2, 2, 12, 12);

        // Probar que el dibujo con símbolos (x, -, +) y con cambios pendientes no genera fallos
        m_style->drawTrafficLightButton(&p, r, QColor(255, 95, 87), 'x', false);
        m_style->drawTrafficLightButton(&p, r, QColor(255, 95, 87), '\0', true); // Unsaved changes dot
        m_style->drawTrafficLightButton(&p, r, QColor(254, 188, 46), '-', false);
        m_style->drawTrafficLightButton(&p, r, QColor(40, 201, 64), '+', false);
    }

    void testModalSheetRendering() {
        QPixmap pix(400, 300);
        QPainter p(&pix);
        QRect sheetRect(50, 0, 300, 150);
        QRect windowRect(0, 0, 400, 300);

        m_style->drawModalSheet(&p, sheetRect, windowRect);
    }

    void testButtonSizeFromContents() {
        QStyleOptionButton opt;
        opt.rect = QRect(0, 0, 80, 20);
        QSize sz = m_style->sizeFromContents(QStyle::CT_PushButton, &opt, QSize(60, 15));
        QVERIFY(sz.height() >= Quartz::StyleMetrics::PushButtonHeight);
    }

private:
    Quartz::Style::QuartzStyle *m_style = nullptr;
};

QTEST_MAIN(TestQuartzStyle)
#include "tst_quartzstyle.moc"
