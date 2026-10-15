#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QSplitter>
#include <QListWidget>
#include <QShortcut>
#include <QDir>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QActionGroup>
#include "findercolumnview.h"
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
    mainWindow.resize(980, 620);

    // Barra de herramientas superior unificada con selector de los 4 modos
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

    // Barra lateral de ubicaciones
    auto *sidebar = new QListWidget(splitter);
    sidebar->setMaximumWidth(180);
    sidebar->setStyleSheet(QStringLiteral(
        "QListWidget { background-color: #EBEBF0; border: none; font-size: 13px; }"
        "QListWidget::item { height: 28px; padding-left: 8px; border-radius: 6px; }"
        "QListWidget::item:selected { background-color: #D1D1D6; color: #1C1C1E; }"
    ));

    auto addSidebarLocation = [&](const QString &title, const QString &path) {
        auto *item = new QListWidgetItem(title, sidebar);
        item->setData(Qt::UserRole, path);
    };

    addSidebarLocation(QStringLiteral("Inicio"), QDir::homePath());
    addSidebarLocation(QStringLiteral("Escritorio"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    addSidebarLocation(QStringLiteral("Documentos"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    addSidebarLocation(QStringLiteral("Descargas"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));

    QString projPath = QDir::homePath() + QStringLiteral("/Proyectos");
    if (!QDir(projPath).exists()) {
        projPath = QDir::homePath();
    }
    addSidebarLocation(QStringLiteral("Proyectos"), projPath);

    // Vista multimodal de Finder
    auto *columnView = new Quartz::Finder::FinderColumnView(splitter);

    // Historial de navegación y cambio de ruta
    QList<QString> history;
    int historyIndex = -1;

    auto updateHistoryButtons = [&]() {
        actBack->setEnabled(historyIndex > 0);
        actForward->setEnabled(historyIndex >= 0 && historyIndex < history.size() - 1);
    };

    auto navigateTo = [&](const QString &path, bool recordHistory = true) {
        QDir dir(path);
        if (!dir.exists()) {
            return;
        }
        QString canonical = dir.canonicalPath();
        if (canonical.isEmpty()) {
            canonical = path;
        }

        if (recordHistory) {
            while (history.size() > historyIndex + 1) {
                history.removeLast();
            }
            history.append(canonical);
            historyIndex = history.size() - 1;
        }

        columnView->setRootPath(canonical);

        QString folderName = QFileInfo(canonical).fileName();
        if (folderName.isEmpty()) {
            folderName = canonical;
        }
        mainWindow.setWindowTitle(QStringLiteral("Finder — %1").arg(folderName));

        // Sincronizar selección en la barra lateral si coincide
        for (int i = 0; i < sidebar->count(); ++i) {
            QListWidgetItem *it = sidebar->item(i);
            if (it->data(Qt::UserRole).toString() == canonical) {
                sidebar->setCurrentItem(it, QItemSelectionModel::ClearAndSelect);
                break;
            }
        }

        updateHistoryButtons();
    };

    QObject::connect(actBack, &QAction::triggered, [&]() {
        if (historyIndex > 0) {
            --historyIndex;
            navigateTo(history[historyIndex], false);
        }
    });

    QObject::connect(actForward, &QAction::triggered, [&]() {
        if (historyIndex >= 0 && historyIndex < history.size() - 1) {
            ++historyIndex;
            navigateTo(history[historyIndex], false);
        }
    });

    QObject::connect(sidebar, &QListWidget::itemClicked, [&](QListWidgetItem *item) {
        if (item) {
            QString path = item->data(Qt::UserRole).toString();
            if (!path.isEmpty()) {
                navigateTo(path, true);
            }
        }
    });

    // Doble clic: entrar en subdirectorios o abrir archivos
    QObject::connect(columnView, &Quartz::Finder::FinderColumnView::fileDoubleClicked, [&](const QString &filePath) {
        QFileInfo fi(filePath);
        if (fi.isDir()) {
            navigateTo(fi.absoluteFilePath(), true);
        } else {
            QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
        }
    });

    // Determinar ruta inicial (soporte para argumentos CLI)
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
            if (argInfo.isDir()) {
                initialPath = argInfo.canonicalFilePath();
            } else {
                initialPath = argInfo.dir().canonicalPath();
            }
        }
    }
    navigateTo(initialPath, true);

    splitter->addWidget(sidebar);
    splitter->addWidget(columnView);
    splitter->setStretchFactor(1, 4);

    mainWindow.setCentralWidget(splitter);

    // Conexión de botones de la barra de herramientas a los modos de vista
    QObject::connect(actIcons, &QAction::triggered, [&]() {
        columnView->setViewMode(Quartz::Finder::FinderViewMode::Icons);
    });
    QObject::connect(actList, &QAction::triggered, [&]() {
        columnView->setViewMode(Quartz::Finder::FinderViewMode::List);
    });
    QObject::connect(actColumns, &QAction::triggered, [&]() {
        columnView->setViewMode(Quartz::Finder::FinderViewMode::Columns);
    });
    QObject::connect(actGallery, &QAction::triggered, [&]() {
        columnView->setViewMode(Quartz::Finder::FinderViewMode::Gallery);
    });

    // Atajos de teclado clásicos de PC/Windows
    auto *scIcons = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_1), &mainWindow);
    QObject::connect(scIcons, &QShortcut::activated, [&]() {
        actIcons->trigger();
    });

    auto *scList = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_2), &mainWindow);
    QObject::connect(scList, &QShortcut::activated, [&]() {
        actList->trigger();
    });

    auto *scColumns = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_3), &mainWindow);
    QObject::connect(scColumns, &QShortcut::activated, [&]() {
        actColumns->trigger();
    });

    auto *scGallery = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_4), &mainWindow);
    QObject::connect(scGallery, &QShortcut::activated, [&]() {
        actGallery->trigger();
    });

    // Ctrl+W: Cerrar ventana
    auto *scClose = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_W), &mainWindow);
    QObject::connect(scClose, &QShortcut::activated, &mainWindow, &QMainWindow::close);

    // Ctrl+Q: Salir de la aplicación
    auto *scQuit = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), &mainWindow);
    QObject::connect(scQuit, &QShortcut::activated, &app, &QApplication::quit);

    // Cliente Quick Look con atajo Espacio
    auto *quickLookClient = new Quartz::Finder::QuickLookClient(&mainWindow);
    auto *spaceShortcut = new QShortcut(QKeySequence(Qt::Key_Space), &mainWindow);
    QObject::connect(spaceShortcut, &QShortcut::activated, [&]() {
        QString sel = columnView->selectedFilePath();
        if (!sel.isEmpty()) {
            quickLookClient->togglePreview(sel, mainWindow.geometry());
        }
    });

    mainWindow.show();
    return app.exec();
}
