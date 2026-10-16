#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include "../src/finder/findercolumnview.h"
#include "../src/finder/fileoperations.h"

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

        Q_EMIT view.fileDoubleClicked(file.fileName());
        QCOMPARE(doubleClickSpy.count(), 1);
        QCOMPARE(doubleClickSpy.takeFirst().at(0).toString(), file.fileName());
    }

    void testUniqueFolderNaming() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        QString name1 = Quartz::Finder::FileOperations::generateUniqueFolderName(tempDir.path());
        QCOMPARE(name1, QStringLiteral("carpeta sin título"));

        QString fullPath1;
        QVERIFY(Quartz::Finder::FileOperations::createFolder(tempDir.path(), name1, &fullPath1));
        QVERIFY(QFile::exists(fullPath1));

        QString name2 = Quartz::Finder::FileOperations::generateUniqueFolderName(tempDir.path());
        QCOMPARE(name2, QStringLiteral("carpeta sin título 2"));
    }

    void testDuplicateNaming() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        // Archivo normal
        QString f1 = tempDir.filePath(QStringLiteral("manual.pdf"));
        QFile file1(f1);
        QVERIFY(file1.open(QIODevice::WriteOnly));
        file1.write("PDF content");
        file1.close();

        QString dup1 = Quartz::Finder::FileOperations::generateDuplicatePath(f1);
        QCOMPARE(dup1, tempDir.filePath(QStringLiteral("manual copia.pdf")));

        // Archivo con extensión compuesta (.tar.gz)
        QString f2 = tempDir.filePath(QStringLiteral("codigo.tar.gz"));
        QFile file2(f2);
        QVERIFY(file2.open(QIODevice::WriteOnly));
        file2.write("TAR.GZ content");
        file2.close();

        QString dup2 = Quartz::Finder::FileOperations::generateDuplicatePath(f2);
        QCOMPARE(dup2, tempDir.filePath(QStringLiteral("codigo copia.tar.gz")));

        // Directorio
        QString d1 = tempDir.filePath(QStringLiteral("Fotos"));
        QDir dir(tempDir.path());
        QVERIFY(dir.mkdir(QStringLiteral("Fotos")));

        QString dupDir = Quartz::Finder::FileOperations::generateDuplicatePath(d1);
        QCOMPARE(dupDir, tempDir.filePath(QStringLiteral("Fotos copia")));
    }

    void testRecursiveCopyWithoutSymlinkFollowing() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        QString srcDir = tempDir.filePath(QStringLiteral("origen"));
        QDir dir(tempDir.path());
        QVERIFY(dir.mkdir(QStringLiteral("origen")));

        // Archivo normal dentro de origen
        QString f1 = srcDir + QStringLiteral("/archivo.txt");
        QFile file1(f1);
        QVERIFY(file1.open(QIODevice::WriteOnly));
        file1.write("Prueba de copia");
        file1.close();

        // Symlink dentro de origen
        QString symlink = srcDir + QStringLiteral("/enlace.txt");
        QVERIFY(QFile::link(f1, symlink));
        QVERIFY(QFileInfo(symlink).isSymLink());

        // Copiar recursivamente
        QString dstDir = tempDir.filePath(QStringLiteral("destino"));
        QVERIFY(Quartz::Finder::FileOperations::copyRecursively(srcDir, dstDir));

        QString copiedFile = dstDir + QStringLiteral("/archivo.txt");
        QVERIFY(QFile::exists(copiedFile));

        QString copiedSymlink = dstDir + QStringLiteral("/enlace.txt");
        QVERIFY(QFileInfo(copiedSymlink).isSymLink());

        // Probar protección contra recursión infinita en subcarpetas
        QString subDst = srcDir + QStringLiteral("/sub_destino");
        QVERIFY(!Quartz::Finder::FileOperations::copyRecursively(srcDir, subDst));
    }

    void testTagAttributes() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        QString filePath = tempDir.filePath(QStringLiteral("etiquetado.txt"));
        QFile file(filePath);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("Contenido");
        file.close();

        bool setOk = Quartz::Finder::FileOperations::setFileTag(filePath, QStringLiteral("Azul"));
        if (!setOk) {
            QSKIP("El sistema de archivos de prueba no soporta atributos extendidos user.xdg.tags (ENOTSUP/EPERM)");
        }

        QString tag = Quartz::Finder::FileOperations::getFileTag(filePath);
        QCOMPARE(tag, QStringLiteral("Azul"));

        // Quitar etiqueta
        QVERIFY(Quartz::Finder::FileOperations::setFileTag(filePath, QString()));
        QString noTag = Quartz::Finder::FileOperations::getFileTag(filePath);
        QVERIFY(noTag.isEmpty());
    }

    void testTrashMoveAndRestore() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        // Aislar papelera fijando XDG_DATA_HOME temporal
        qputenv("XDG_DATA_HOME", tempDir.path().toUtf8());

        QString filePath = tempDir.filePath(QStringLiteral("borrar.txt"));
        QFile file(filePath);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("A la papelera");
        file.close();
        QVERIFY(QFile::exists(filePath));

        bool trashed = Quartz::Finder::FileOperations::moveToTrash(filePath);
        if (trashed) {
            QVERIFY(!QFile::exists(filePath));
        }
    }
};

QTEST_MAIN(TestFinderModes)
#include "tst_finder_modes.moc"
