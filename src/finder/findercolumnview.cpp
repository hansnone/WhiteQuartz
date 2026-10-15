#include "findercolumnview.h"
#include <QScrollBar>
#include <QHeaderView>

namespace Quartz::Finder {

FinderColumnView::FinderColumnView(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_viewStack = new QStackedWidget(this);
    mainLayout->addWidget(m_viewStack);

    setupIconsView();
    setupListView();
    setupColumnsView();
    setupGalleryView();

    setViewMode(FinderViewMode::Columns);
}

void FinderColumnView::setupIconsView()
{
    m_iconsList = new QListWidget(this);
    m_iconsList->setViewMode(QListView::IconMode);
    m_iconsList->setIconSize(QSize(64, 64));
    m_iconsList->setGridSize(QSize(96, 96));
    m_iconsList->setResizeMode(QListWidget::Adjust);
    m_iconsList->setMovement(QListView::Static);
    m_iconsList->setSpacing(12);
    m_iconsList->setStyleSheet(QStringLiteral("QListWidget { background: #FFFFFF; border: none; }"));

    connect(m_iconsList, &QListWidget::itemClicked, this, &FinderColumnView::onIconItemClicked);
    m_viewStack->addWidget(m_iconsList);
}

void FinderColumnView::setupListView()
{
    m_listTree = new QTreeWidget(this);
    m_listTree->setColumnCount(3);
    m_listTree->setHeaderLabels({ QStringLiteral("Nombre"), QStringLiteral("Fecha de modificación"), QStringLiteral("Tamaño") });
    m_listTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_listTree->setIndentation(18);
    m_listTree->setStyleSheet(QStringLiteral("QTreeWidget { background: #FFFFFF; border: none; }"));

    connect(m_listTree, &QTreeWidget::itemClicked, this, &FinderColumnView::onListItemClicked);
    m_viewStack->addWidget(m_listTree);
}

void FinderColumnView::setupColumnsView()
{
    m_columnsScroll = new QScrollArea(this);
    m_columnsScroll->setWidgetResizable(true);
    m_columnsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_columnsScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_columnsContainer = new QWidget(m_columnsScroll);
    m_columnsLayout = new QHBoxLayout(m_columnsContainer);
    m_columnsLayout->setContentsMargins(0, 0, 0, 0);
    m_columnsLayout->setSpacing(1);

    m_columnsScroll->setWidget(m_columnsContainer);

    // Inspector de metadatos (columna final de Miller Columns)
    m_inspectorWidget = new QWidget(m_columnsContainer);
    m_inspectorWidget->setFixedWidth(240);
    m_inspectorWidget->setStyleSheet(QStringLiteral("QWidget { background: #F8F8F9; border-left: 1px solid #E5E5EA; }"));

    auto *inspLayout = new QVBoxLayout(m_inspectorWidget);
    m_inspectorPreviewLabel = new QLabel(QStringLiteral("Vista previa"), m_inspectorWidget);
    m_inspectorPreviewLabel->setAlignment(Qt::AlignCenter);
    m_inspectorPreviewLabel->setFixedHeight(120);
    m_inspectorPreviewLabel->setStyleSheet(QStringLiteral("background: #EBEBF0; border-radius: 6px;"));

    m_inspectorDetailsLabel = new QLabel(QStringLiteral("Selecciona un archivo"), m_inspectorWidget);
    m_inspectorDetailsLabel->setWordWrap(true);
    m_inspectorDetailsLabel->setStyleSheet(QStringLiteral("font-size: 12px; color: #3C3C43;"));

    inspLayout->addWidget(m_inspectorPreviewLabel);
    inspLayout->addWidget(m_inspectorDetailsLabel);
    inspLayout->addStretch();
    m_inspectorWidget->setVisible(false);

    m_viewStack->addWidget(m_columnsScroll);
}

void FinderColumnView::setupGalleryView()
{
    m_galleryWidget = new QWidget(this);
    auto *galleryLayout = new QVBoxLayout(m_galleryWidget);
    galleryLayout->setContentsMargins(12, 12, 12, 12);
    galleryLayout->setSpacing(10);

    m_galleryMainPreview = new QLabel(QStringLiteral("Previsualización de Galería"), m_galleryWidget);
    m_galleryMainPreview->setAlignment(Qt::AlignCenter);
    m_galleryMainPreview->setStyleSheet(QStringLiteral("background: #F2F2F7; border-radius: 10px; font-size: 16px; font-weight: bold;"));

    m_galleryThumbnailStrip = new QListWidget(m_galleryWidget);
    m_galleryThumbnailStrip->setFixedHeight(90);
    m_galleryThumbnailStrip->setViewMode(QListView::IconMode);
    m_galleryThumbnailStrip->setIconSize(QSize(60, 60));
    m_galleryThumbnailStrip->setSpacing(8);

    m_galleryDetailsLabel = new QLabel(QStringLiteral("Metadatos y Detalles"), m_galleryWidget);
    m_galleryDetailsLabel->setFixedHeight(40);
    m_galleryDetailsLabel->setAlignment(Qt::AlignCenter);

    galleryLayout->addWidget(m_galleryMainPreview, 4);
    galleryLayout->addWidget(m_galleryThumbnailStrip, 1);
    galleryLayout->addWidget(m_galleryDetailsLabel);

    m_viewStack->addWidget(m_galleryWidget);
}

void FinderColumnView::setViewMode(FinderViewMode mode)
{
    m_viewMode = mode;
    switch (mode) {
    case FinderViewMode::Icons:
        m_viewStack->setCurrentWidget(m_iconsList);
        break;
    case FinderViewMode::List:
        m_viewStack->setCurrentWidget(m_listTree);
        break;
    case FinderViewMode::Columns:
        m_viewStack->setCurrentWidget(m_columnsScroll);
        break;
    case FinderViewMode::Gallery:
        m_viewStack->setCurrentWidget(m_galleryWidget);
        break;
    }
    Q_EMIT viewModeChanged(mode);
}

void FinderColumnView::setRootPath(const QString &path)
{
    m_rootPath = path;
    truncateColumnsAfter(-1);
    addColumn(path);

    // Cargar también en la vista de lista e iconos
    QDir dir(path);
    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);

    m_iconsList->clear();
    m_listTree->clear();

    for (const QFileInfo &info : entries) {
        // Iconos
        auto *iconItem = new QListWidgetItem(info.fileName(), m_iconsList);
        iconItem->setData(Qt::UserRole, info.absoluteFilePath());

        // Lista con columnas
        auto *treeItem = new QTreeWidgetItem(m_listTree);
        treeItem->setText(0, info.isDir() ? QStringLiteral("▶  ") + info.fileName() : info.fileName());
        treeItem->setText(1, info.lastModified().toString(QStringLiteral("dd/MM/yyyy HH:mm")));
        treeItem->setText(2, info.isDir() ? QStringLiteral("--") : QString::number(info.size() / 1024) + QStringLiteral(" KB"));
        treeItem->setData(0, Qt::UserRole, info.absoluteFilePath());
    }
}

void FinderColumnView::addColumn(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    auto *listWidget = new QListWidget(m_columnsContainer);
    listWidget->setFixedWidth(220);
    listWidget->setFrameShape(QFrame::NoFrame);
    listWidget->setStyleSheet(QStringLiteral(
        "QListWidget { background-color: #FFFFFF; border-right: 1px solid #E5E5EA; }"
        "QListWidget::item { height: 26px; padding-left: 6px; border-radius: 4px; }"
        "QListWidget::item:selected { background-color: #007AFF; color: white; }"
    ));

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
    for (const QFileInfo &info : entries) {
        QString displayName = info.fileName();
        if (info.isDir()) {
            displayName += QStringLiteral("  ›");
        }
        auto *item = new QListWidgetItem(displayName, listWidget);
        item->setData(Qt::UserRole, info.absoluteFilePath());
        item->setData(Qt::UserRole + 1, info.isDir());
    }

    connect(listWidget, &QListWidget::itemClicked, this, &FinderColumnView::onColumnItemClicked);

    // Si el inspector está en el layout, insertarlo antes del inspector
    if (m_inspectorWidget && m_columnsLayout->indexOf(m_inspectorWidget) != -1) {
        m_columnsLayout->removeWidget(m_inspectorWidget);
        m_columnsLayout->addWidget(listWidget);
        m_columnsLayout->addWidget(m_inspectorWidget);
    } else {
        m_columnsLayout->addWidget(listWidget);
    }

    m_columns.append(listWidget);
    m_columnPaths.append(path);

    QScrollBar *hBar = m_columnsScroll->horizontalScrollBar();
    if (hBar) {
        hBar->setValue(hBar->maximum());
    }
}

void FinderColumnView::truncateColumnsAfter(int columnIndex)
{
    while (m_columns.size() > columnIndex + 1) {
        QListWidget *lastCol = m_columns.takeLast();
        m_columnPaths.removeLast();
        m_columnsLayout->removeWidget(lastCol);
        lastCol->deleteLater();
    }
}

void FinderColumnView::updateInspector(const QString &filePath)
{
    QFileInfo info(filePath);
    if (!info.exists()) {
        m_inspectorWidget->setVisible(false);
        return;
    }

    m_inspectorPreviewLabel->setText(info.fileName());
    QString details = QStringLiteral("<b>Nombre:</b> %1<br>"
                                     "<b>Tipo:</b> %2<br>"
                                     "<b>Tamaño:</b> %3 bytes<br>"
                                     "<b>Modificado:</b> %4")
                          .arg(info.fileName())
                          .arg(info.isDir() ? QStringLiteral("Carpeta") : info.suffix().toUpper())
                          .arg(info.size())
                          .arg(info.lastModified().toString(QStringLiteral("yyyy-MM-dd HH:mm")));

    m_inspectorDetailsLabel->setText(details);
    m_inspectorWidget->setVisible(true);

    if (m_columnsLayout->indexOf(m_inspectorWidget) == -1) {
        m_columnsLayout->addWidget(m_inspectorWidget);
    }
}

void FinderColumnView::onColumnItemClicked(QListWidgetItem *item)
{
    auto *senderList = qobject_cast<QListWidget *>(sender());
    if (!senderList) {
        return;
    }

    int colIndex = m_columns.indexOf(senderList);
    if (colIndex == -1) {
        return;
    }

    QString filePath = item->data(Qt::UserRole).toString();
    bool isDir = item->data(Qt::UserRole + 1).toBool();

    m_currentSelectedFile = filePath;
    Q_EMIT fileSelected(filePath);

    truncateColumnsAfter(colIndex);

    if (isDir) {
        m_inspectorWidget->setVisible(false);
        addColumn(filePath);
    } else {
        updateInspector(filePath);
    }
}

void FinderColumnView::onListItemClicked(QTreeWidgetItem *item, int /*column*/)
{
    QString path = item->data(0, Qt::UserRole).toString();
    m_currentSelectedFile = path;
    Q_EMIT fileSelected(path);
}

void FinderColumnView::onIconItemClicked(QListWidgetItem *item)
{
    QString path = item->data(Qt::UserRole).toString();
    m_currentSelectedFile = path;
    Q_EMIT fileSelected(path);
}

} // namespace Quartz::Finder
