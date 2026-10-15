#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QSplitter>
#include <QListWidget>
#include <QShortcut>
#include <QDir>
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
    toolbar->addAction(QStringLiteral("‹"));
    toolbar->addAction(QStringLiteral("›"));
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

    new QListWidgetItem(QStringLiteral("Inicio"), sidebar);
    new QListWidgetItem(QStringLiteral("Escritorio"), sidebar);
    new QListWidgetItem(QStringLiteral("Documentos"), sidebar);
    new QListWidgetItem(QStringLiteral("Descargas"), sidebar);
    new QListWidgetItem(QStringLiteral("Proyectos"), sidebar);

    // Vista multimodal de Finder
    auto *columnView = new Quartz::Finder::FinderColumnView(splitter);
    columnView->setRootPath(QDir::homePath());

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
