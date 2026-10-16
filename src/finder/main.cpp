#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QSplitter>
#include <QListWidget>
#include <QTabWidget>
#include <QTabBar>
#include <QShortcut>
#include <QDir>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QActionGroup>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QStorageInfo>
#include <QDBusMessage>
#include <QDBusConnection>
#include "findercolumnview.h"
#include "fileoperations.h"
#include "quicklookclient.h"
#include "../style/quartzstyle.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("QuartzFinder"));
    app.setOrganizationName(QStringLiteral("QuartzDE"));

    QApplication::setStyle(new Quartz::Style::QuartzStyle);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle(QStringLiteral("Finder"));
    mainWindow.resize(1020, 640);

    // Barra de herramientas superior unificada
    QToolBar *toolbar = mainWindow.addToolBar(QStringLiteral("Barra de Navegación"));
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    auto *actBack = toolbar->addAction(QStringLiteral("‹"));
    actBack->setToolTip(QStringLiteral("Atrás (Alt+Izquierda)"));
    actBack->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Left));

    auto *actForward = toolbar->addAction(QStringLiteral("›"));
    actForward->setToolTip(QStringLiteral("Adelante (Alt+Derecha)"));
    actForward->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Right));
    toolbar->addSeparator();

    auto *modeGroup = new QActionGroup(&mainWindow);
    auto *actIcons = toolbar->addAction(QStringLiteral("Iconos (Ctrl+1)"));
    auto *actList = toolbar->addAction(QStringLiteral("Lista (Ctrl+2)"));
    auto *actColumns = toolbar->addAction(QStringLiteral("Columnas (Ctrl+3)"));
    auto *actGallery = toolbar->addAction(QStringLiteral("Galería (Ctrl+4)"));

    for (auto *act : { actIcons, actList, actColumns, actGallery }) {
        act->setCheckable(true);
        modeGroup->addAction(act);
    }
    actColumns->setChecked(true);

    auto *splitter = new QSplitter(&mainWindow);

    // Barra lateral de ubicaciones y etiquetas
    auto *sidebar = new QListWidget(splitter);
    sidebar->setMaximumWidth(200);
    sidebar->setStyleSheet(QStringLiteral(
        "QListWidget { background-color: #EBEBF0; border: none; font-size: 13px; }"
        "QListWidget::item { height: 26px; padding-left: 8px; border-radius: 6px; margin: 1px 4px; }"
        "QListWidget::item:selected { background-color: #D1D1D6; color: #1C1C1E; font-weight: bold; }"
    ));

    auto addCategoryHeader = [&](const QString &title) {
        auto *item = new QListWidgetItem(title, sidebar);
        item->setFlags(Qt::NoItemFlags);
        QFont f = item->font();
        f.setBold(true);
        f.setPixelSize(11);
        item->setFont(f);
        item->setForeground(QColor(QStringLiteral("#8E8E93")));
        item->setData(Qt::UserRole, QString());
    };

    auto addSidebarLocation = [&](const QString &title, const QString &path, const QString &tag = QString()) {
        auto *item = new QListWidgetItem(title, sidebar);
        item->setData(Qt::UserRole, path);
        item->setData(Qt::UserRole + 1, tag);
    };

    addCategoryHeader(QStringLiteral("FAVORITOS"));
    addSidebarLocation(QStringLiteral("🏠  Inicio"), QDir::homePath());
    addSidebarLocation(QStringLiteral("🖥️  Escritorio"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    addSidebarLocation(QStringLiteral("📄  Documentos"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    addSidebarLocation(QStringLiteral("⬇️  Descargas"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));

    QString projPath = QDir::homePath() + QStringLiteral("/Proyectos");
    if (!QDir(projPath).exists()) {
        projPath = QDir::homePath();
    }
    addSidebarLocation(QStringLiteral("📁  Proyectos"), projPath);

    // Ubicaciones de almacenamiento
    addCategoryHeader(QStringLiteral("UBICACIONES"));
    addSidebarLocation(QStringLiteral("💾  Macintosh HD"), QStringLiteral("/"));

    // Volúmenes montados
    const auto volumes = QStorageInfo::mountedVolumes();
    for (const QStorageInfo &vol : volumes) {
        if (!vol.isValid() || !vol.isReady()) continue;
        QString root = vol.rootPath();
        if (root == QStringLiteral("/") || root.startsWith(QStringLiteral("/proc")) ||
            root.startsWith(QStringLiteral("/sys")) || root.startsWith(QStringLiteral("/run")) ||
            root.startsWith(QStringLiteral("/dev")) || vol.fileSystemType() == "tmpfs" ||
            vol.fileSystemType() == "devtmpfs" || vol.fileSystemType() == "overlay") {
            continue;
        }
        QString name = vol.name().isEmpty() ? QFileInfo(root).fileName() : vol.name();
        addSidebarLocation(QStringLiteral("💽  ") + name, root);
    }

    // Etiquetas
    addCategoryHeader(QStringLiteral("ETIQUETAS"));
    addSidebarLocation(QStringLiteral("🔴  Rojo"), QString(), QStringLiteral("Rojo"));
    addSidebarLocation(QStringLiteral("🟠  Naranja"), QString(), QStringLiteral("Naranja"));
    addSidebarLocation(QStringLiteral("🟡  Amarillo"), QString(), QStringLiteral("Amarillo"));
    addSidebarLocation(QStringLiteral("🟢  Verde"), QString(), QStringLiteral("Verde"));
    addSidebarLocation(QStringLiteral("🔵  Azul"), QString(), QStringLiteral("Azul"));
    addSidebarLocation(QStringLiteral("🟣  Morado"), QString(), QStringLiteral("Morado"));
    addSidebarLocation(QStringLiteral("⚪  Todas"), QString(), QStringLiteral("ALL"));

    // Gestor de Pestañas
    auto *tabWidget = new QTabWidget(splitter);
    tabWidget->setTabsClosable(true);
    tabWidget->tabBar()->setAutoHide(true);
    tabWidget->setDocumentMode(true);
    tabWidget->setStyleSheet(QStringLiteral(
        "QTabWidget::pane { border: none; background: #FFFFFF; }"
        "QTabBar::tab { height: 26px; padding: 0 16px; background: #E5E5EA; border-top-left-radius: 6px; border-top-right-radius: 6px; margin-right: 2px; font-size: 12px; }"
        "QTabBar::tab:selected { background: #FFFFFF; font-weight: bold; }"
    ));

    // Estructura de estado por pestaña
    struct TabState {
        Quartz::Finder::FinderColumnView *view = nullptr;
        QList<QString> history;
        int historyIndex = -1;
    };
    QList<TabState> tabs;

    auto currentTab = [&]() -> TabState* {
        int idx = tabWidget->currentIndex();
        if (idx >= 0 && idx < tabs.size()) {
            return &tabs[idx];
        }
        return nullptr;
    };

    auto updateToolbar = [&]() {
        TabState *t = currentTab();
        if (!t) return;
        actBack->setEnabled(t->historyIndex > 0);
        actForward->setEnabled(t->historyIndex >= 0 && t->historyIndex < t->history.size() - 1);

        Quartz::Finder::FinderViewMode m = t->view->currentViewMode();
        switch (m) {
        case Quartz::Finder::FinderViewMode::Icons: actIcons->setChecked(true); break;
        case Quartz::Finder::FinderViewMode::List: actList->setChecked(true); break;
        case Quartz::Finder::FinderViewMode::Columns: actColumns->setChecked(true); break;
        case Quartz::Finder::FinderViewMode::Gallery: actGallery->setChecked(true); break;
        }

        QString folderName = QFileInfo(t->view->rootPath()).fileName();
        if (folderName.isEmpty()) folderName = t->view->rootPath();
        mainWindow.setWindowTitle(QStringLiteral("Finder — %1").arg(folderName));
        tabWidget->setTabText(tabWidget->currentIndex(), folderName);
    };

    auto navigateCurrentTab = [&](const QString &path, bool recordHistory = true) {
        TabState *t = currentTab();
        if (!t) return;
        QDir dir(path);
        if (!dir.exists()) return;
        QString canonical = dir.canonicalPath();
        if (canonical.isEmpty()) canonical = path;

        if (recordHistory) {
            while (t->history.size() > t->historyIndex + 1) {
                t->history.removeLast();
            }
            t->history.append(canonical);
            t->historyIndex = t->history.size() - 1;
        }

        t->view->setRootPath(canonical);
        updateToolbar();
    };

    auto addNewTab = [&](const QString &path) {
        auto *view = new Quartz::Finder::FinderColumnView(tabWidget);
        TabState ts;
        ts.view = view;
        tabs.append(ts);

        int tabIdx = tabWidget->addTab(view, QFileInfo(path).fileName());
        tabWidget->setCurrentIndex(tabIdx);

        QObject::connect(view, &Quartz::Finder::FinderColumnView::pathNavigated, [&](const QString &p) {
            navigateCurrentTab(p, true);
        });

        QObject::connect(view, &Quartz::Finder::FinderColumnView::fileDoubleClicked, [&](const QString &filePath) {
            QFileInfo fi(filePath);
            if (fi.isDir()) {
                navigateCurrentTab(fi.absoluteFilePath(), true);
            } else {
                QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
            }
        });

        navigateCurrentTab(path, true);
    };

    // Cerrar pestañas
    QObject::connect(tabWidget, &QTabWidget::tabCloseRequested, [&](int index) {
        if (tabWidget->count() > 1) {
            QWidget *w = tabWidget->widget(index);
            tabs.removeAt(index);
            tabWidget->removeTab(index);
            w->deleteLater();
            updateToolbar();
        } else {
            mainWindow.close();
        }
    });

    QObject::connect(tabWidget, &QTabWidget::currentChanged, [&](int) {
        updateToolbar();
    });

    // Conexión atrás / adelante
    QObject::connect(actBack, &QAction::triggered, [&]() {
        TabState *t = currentTab();
        if (t && t->historyIndex > 0) {
            --t->historyIndex;
            navigateCurrentTab(t->history[t->historyIndex], false);
        }
    });

    QObject::connect(actForward, &QAction::triggered, [&]() {
        TabState *t = currentTab();
        if (t && t->historyIndex >= 0 && t->historyIndex < t->history.size() - 1) {
            ++t->historyIndex;
            navigateCurrentTab(t->history[t->historyIndex], false);
        }
    });

    // Clic en sidebar
    QObject::connect(sidebar, &QListWidget::itemClicked, [&](QListWidgetItem *item) {
        if (!item) return;
        QString path = item->data(Qt::UserRole).toString();
        QString tag = item->data(Qt::UserRole + 1).toString();

        TabState *t = currentTab();
        if (!t) return;

        if (!tag.isEmpty()) {
            if (tag == QStringLiteral("ALL")) {
                t->view->setTagFilter(QString());
            } else {
                t->view->setTagFilter(tag);
            }
        } else if (!path.isEmpty()) {
            t->view->setTagFilter(QString());
            navigateCurrentTab(path, true);
        }
    });

    // Conexión de modos de vista
    QObject::connect(actIcons, &QAction::triggered, [&]() {
        if (auto *t = currentTab()) t->view->setViewMode(Quartz::Finder::FinderViewMode::Icons);
    });
    QObject::connect(actList, &QAction::triggered, [&]() {
        if (auto *t = currentTab()) t->view->setViewMode(Quartz::Finder::FinderViewMode::List);
    });
    QObject::connect(actColumns, &QAction::triggered, [&]() {
        if (auto *t = currentTab()) t->view->setViewMode(Quartz::Finder::FinderViewMode::Columns);
    });
    QObject::connect(actGallery, &QAction::triggered, [&]() {
        if (auto *t = currentTab()) t->view->setViewMode(Quartz::Finder::FinderViewMode::Gallery);
    });

    // Operaciones de Archivo (Acciones de menú contextual y atajos)
    auto *fileOps = new Quartz::Finder::FileOperations(&mainWindow);

    auto actionNewFolder = [&]() {
        TabState *t = currentTab();
        if (!t) return;
        QString created;
        if (Quartz::Finder::FileOperations::createFolder(t->view->rootPath(), QString(), &created)) {
            fileOps->undoStack()->push(new Quartz::Finder::CreateFolderCommand(created));
            t->view->reloadCurrentDirectory();
        }
    };

    auto actionCopy = [&]() {
        TabState *t = currentTab();
        if (!t) return;
        Quartz::Finder::FileOperations::copyToClipboard(t->view->selectedFilePaths(), false);
    };

    auto actionCut = [&]() {
        TabState *t = currentTab();
        if (!t) return;
        Quartz::Finder::FileOperations::copyToClipboard(t->view->selectedFilePaths(), true);
    };

    auto actionPaste = [&]() {
        TabState *t = currentTab();
        if (!t) return;
        QStringList paths = Quartz::Finder::FileOperations::clipboardFilePaths();
        if (paths.isEmpty()) return;

        bool isCut = Quartz::Finder::FileOperations::isCutOperationInClipboard();
        if (isCut) {
            fileOps->moveFilesAsync(paths, t->view->rootPath());
        } else {
            fileOps->copyFilesAsync(paths, t->view->rootPath());
        }
    };

    auto actionDuplicate = [&]() {
        TabState *t = currentTab();
        if (!t) return;
        for (const QString &p : t->view->selectedFilePaths()) {
            Quartz::Finder::FileOperations::duplicateFileOrDirectory(p);
        }
        t->view->reloadCurrentDirectory();
    };

    auto actionRename = [&]() {
        TabState *t = currentTab();
        if (!t) return;
        QString current = t->view->selectedFilePath();
        if (current.isEmpty()) return;
        QFileInfo fi(current);
        bool ok = false;
        QString newName = QInputDialog::getText(&mainWindow, QStringLiteral("Renombrar"),
                                                QStringLiteral("Nuevo nombre:"), QLineEdit::Normal,
                                                fi.fileName(), &ok);
        if (ok && !newName.isEmpty() && newName != fi.fileName()) {
            QString newP;
            if (Quartz::Finder::FileOperations::renameFileOrDirectory(current, newName, &newP)) {
                fileOps->undoStack()->push(new Quartz::Finder::RenameCommand(current, newP));
                t->view->reloadCurrentDirectory();
            }
        }
    };

    auto actionTrash = [&]() {
        TabState *t = currentTab();
        if (!t) return;
        for (const QString &p : t->view->selectedFilePaths()) {
            Quartz::Finder::FileOperations::moveToTrash(p);
        }
        t->view->reloadCurrentDirectory();
    };

    QObject::connect(fileOps, &Quartz::Finder::FileOperations::operationFinished, [&](bool, const QString &) {
        if (auto *t = currentTab()) {
            t->view->reloadCurrentDirectory();
        }
    });

    // Menú contextual
    mainWindow.setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(&mainWindow, &QMainWindow::customContextMenuRequested, [&](const QPoint &pos) {
        TabState *t = currentTab();
        if (!t) return;

        QMenu menu(&mainWindow);
        QString selPath = t->view->selectedFilePath();

        if (!selPath.isEmpty()) {
            menu.addAction(QStringLiteral("Abrir"), [&]() {
                QDesktopServices::openUrl(QUrl::fromLocalFile(selPath));
            });
            menu.addAction(QStringLiteral("Obtener información (Ctrl+I)"), [&]() {
                QMessageBox::information(&mainWindow, QStringLiteral("Información"),
                                         QStringLiteral("Ruta: %1\nTamaño: %2 bytes")
                                             .arg(selPath)
                                             .arg(QFileInfo(selPath).size()));
            });
            menu.addSeparator();
            menu.addAction(QStringLiteral("Duplicar (Ctrl+D)"), actionDuplicate);
            menu.addAction(QStringLiteral("Renombrar (F2)"), actionRename);
            menu.addAction(QStringLiteral("Mover a la Papelera (Delete)"), actionTrash);
            menu.addSeparator();
            menu.addAction(QStringLiteral("Copiar (Ctrl+C)"), actionCopy);

            // Submenú de Etiquetas de color
            QMenu *tagsSubMenu = menu.addMenu(QStringLiteral("Etiquetas"));
            for (const QString &tag : Quartz::Finder::FileOperations::standardTagNames()) {
                tagsSubMenu->addAction(tag, [&, selPath, tag]() {
                    Quartz::Finder::FileOperations::setFileTag(selPath, tag);
                    t->view->reloadCurrentDirectory();
                });
            }
            tagsSubMenu->addAction(QStringLiteral("Quitar etiqueta"), [&, selPath]() {
                Quartz::Finder::FileOperations::setFileTag(selPath, QString());
                t->view->reloadCurrentDirectory();
            });
        }

        menu.addSeparator();
        menu.addAction(QStringLiteral("Nueva carpeta (Ctrl+Shift+N)"), actionNewFolder);
        if (!Quartz::Finder::FileOperations::clipboardFilePaths().isEmpty()) {
            menu.addAction(QStringLiteral("Pegar (Ctrl+V)"), actionPaste);
        }

        menu.exec(mainWindow.mapToGlobal(pos));
    });

    // Atajos de Teclado PC/Windows
    auto *scNewFolder = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N), &mainWindow);
    QObject::connect(scNewFolder, &QShortcut::activated, actionNewFolder);

    auto *scCopy = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), &mainWindow);
    QObject::connect(scCopy, &QShortcut::activated, actionCopy);

    auto *scCut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), &mainWindow);
    QObject::connect(scCut, &QShortcut::activated, actionCut);

    auto *scPaste = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_V), &mainWindow);
    QObject::connect(scPaste, &QShortcut::activated, actionPaste);

    auto *scDuplicate = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), &mainWindow);
    QObject::connect(scDuplicate, &QShortcut::activated, actionDuplicate);

    auto *scRename = new QShortcut(QKeySequence(Qt::Key_F2), &mainWindow);
    QObject::connect(scRename, &QShortcut::activated, actionRename);

    auto *scTrash = new QShortcut(QKeySequence(Qt::Key_Delete), &mainWindow);
    QObject::connect(scTrash, &QShortcut::activated, actionTrash);

    auto *scUndo = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), &mainWindow);
    QObject::connect(scUndo, &QShortcut::activated, [&]() {
        fileOps->undoStack()->undo();
        if (auto *t = currentTab()) t->view->reloadCurrentDirectory();
    });

    // Modos de Vista
    auto *scIcons = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_1), &mainWindow);
    QObject::connect(scIcons, &QShortcut::activated, [&]() { actIcons->trigger(); });

    auto *scList = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_2), &mainWindow);
    QObject::connect(scList, &QShortcut::activated, [&]() { actList->trigger(); });

    auto *scColumns = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_3), &mainWindow);
    QObject::connect(scColumns, &QShortcut::activated, [&]() { actColumns->trigger(); });

    auto *scGallery = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_4), &mainWindow);
    QObject::connect(scGallery, &QShortcut::activated, [&]() { actGallery->trigger(); });

    // Pestañas: Ctrl+T (nueva), Ctrl+W (cerrar), Ctrl+Tab (siguiente)
    auto *scNewTab = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), &mainWindow);
    QObject::connect(scNewTab, &QShortcut::activated, [&]() {
        if (auto *t = currentTab()) {
            addNewTab(t->view->rootPath());
        }
    });

    auto *scCloseTab = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_W), &mainWindow);
    QObject::connect(scCloseTab, &QShortcut::activated, [&]() {
        int idx = tabWidget->currentIndex();
        if (tabWidget->count() > 1) {
            tabWidget->tabCloseRequested(idx);
        } else {
            mainWindow.close();
        }
    });

    auto *scNextTab = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Tab), &mainWindow);
    QObject::connect(scNextTab, &QShortcut::activated, [&]() {
        if (tabWidget->count() > 1) {
            tabWidget->setCurrentIndex((tabWidget->currentIndex() + 1) % tabWidget->count());
        }
    });

    auto *scPrevTab = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab), &mainWindow);
    QObject::connect(scPrevTab, &QShortcut::activated, [&]() {
        if (tabWidget->count() > 1) {
            tabWidget->setCurrentIndex((tabWidget->currentIndex() - 1 + tabWidget->count()) % tabWidget->count());
        }
    });

    // Ctrl+Q: Salir de la aplicación
    auto *scQuit = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), &mainWindow);
    QObject::connect(scQuit, &QShortcut::activated, &app, &QApplication::quit);

    // Cliente Quick Look con atajo Espacio
    auto *quickLookClient = new Quartz::Finder::QuickLookClient(&mainWindow);
    auto *spaceShortcut = new QShortcut(QKeySequence(Qt::Key_Space), &mainWindow);
    QObject::connect(spaceShortcut, &QShortcut::activated, [&]() {
        if (auto *t = currentTab()) {
            QString sel = t->view->selectedFilePath();
            if (!sel.isEmpty()) {
                quickLookClient->togglePreview(sel, mainWindow.geometry());
            }
        }
    });

    // Determinar ruta inicial
    QString initialPath = QDir::homePath();
    if (argc > 1) {
        QString argPath = QString::fromLocal8Bit(argv[1]);
        if (argPath.startsWith(QLatin1String("~/"))) {
            argPath = QDir::homePath() + argPath.mid(1);
        } else if (argPath == QLatin1String("~")) {
            argPath = QDir::homePath();
        }
        QFileInfo argInfo(argPath);
        if (argInfo.exists()) {
            initialPath = argInfo.isDir() ? argInfo.canonicalFilePath() : argInfo.dir().canonicalPath();
        }
    }

    addNewTab(initialPath);

    splitter->addWidget(sidebar);
    splitter->addWidget(tabWidget);
    splitter->setStretchFactor(1, 4);

    mainWindow.setCentralWidget(splitter);
    mainWindow.show();

    return app.exec();
}
