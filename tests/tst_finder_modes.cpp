#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QFile>
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

    void testGalleryPopulationAndDirectoryEntries() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        // Crear archivos de prueba
        QFile file1(tempDir.filePath(QStringLiteral("documento.txt")));
        QVERIFY(file1.open(QIODevice::WriteOnly));
        file1.write("QuartzDE Documento");
        file1.close();

        QFile file2(tempDir.filePath(QStringLiteral("imagen.png")));
        QVERIFY(file2.open(QIODevice::WriteOnly));
        file2.write("PNG DATA");
        file2.close();

        QDir dir(tempDir.path());
        QVERIFY(dir.mkdir(QStringLiteral("Subcarpeta")));

        Quartz::Finder::FinderColumnView view;
        view.setRootPath(tempDir.path());

        QCOMPARE(view.rootPath(), tempDir.path());
        QCOMPARE(view.galleryItemCount(), 3);
        QCOMPARE(view.iconItemCount(), 3);
        QCOMPARE(view.listItemCount(), 3);
        QVERIFY(view.columnCount() >= 1);
        QVERIFY(!view.selectedFilePath().isEmpty());
    }

    void testSignalsEmission() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        QFile file(tempDir.filePath(QStringLiteral("demo.txt")));
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("Demo");
        file.close();

        Quartz::Finder::FinderColumnView view;
        QSignalSpy doubleClickSpy(&view, &Quartz::Finder::FinderColumnView::fileDoubleClicked);
        QSignalSpy selectSpy(&view, &Quartz::Finder::FinderColumnView::fileSelected);

        view.setRootPath(tempDir.path());
        QVERIFY(selectSpy.count() >= 1);

        // Emitir señal de doble clic y verificar captura
        Q_EMIT view.fileDoubleClicked(file.fileName());
        QCOMPARE(doubleClickSpy.count(), 1);
        QCOMPARE(doubleClickSpy.takeFirst().at(0).toString(), file.fileName());
    }
};

QTEST_MAIN(TestFinderModes)
#include "tst_finder_modes.moc"
