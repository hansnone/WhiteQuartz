#include <QTest>
#include <QApplication>
#include "../src/finder/findercolumnview.h"

class TestFinderModes : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testInitialMode() {
        Quartz::Finder::FinderColumnView view;
        QCOMPARE(static_cast<int>(view.currentViewMode()), static_cast<int>(Quartz::Finder::FinderViewMode::Columns));
    }

    void testModeSwitching() {
        Quartz::Finder::FinderColumnView view;

        view.setViewMode(Quartz::Finder::FinderViewMode::Icons);
        QCOMPARE(static_cast<int>(view.currentViewMode()), static_cast<int>(Quartz::Finder::FinderViewMode::Icons));

        view.setViewMode(Quartz::Finder::FinderViewMode::List);
        QCOMPARE(static_cast<int>(view.currentViewMode()), static_cast<int>(Quartz::Finder::FinderViewMode::List));

        view.setViewMode(Quartz::Finder::FinderViewMode::Gallery);
        QCOMPARE(static_cast<int>(view.currentViewMode()), static_cast<int>(Quartz::Finder::FinderViewMode::Gallery));

        view.setViewMode(Quartz::Finder::FinderViewMode::Columns);
        QCOMPARE(static_cast<int>(view.currentViewMode()), static_cast<int>(Quartz::Finder::FinderViewMode::Columns));
    }
};

QTEST_MAIN(TestFinderModes)
#include "tst_finder_modes.moc"
