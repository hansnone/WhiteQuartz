#include "findercolumnview.h"
#include "fileoperations.h"
#include "tagcircledelegate.h"
#include <QFileSystemModel>
#include <QScrollBar>
#include <QHeaderView>
#include <QStorageInfo>
#include <QImageReader>
#include <QPixmapCache>
#include <QtConcurrentRun>

namespace Quartz::Finder {

FinderColumnView::FinderColumnView(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_viewStack = new QStackedWidget(this);
    mainLayout->addWidget(m_viewStack, 1);

    setupIconsView();
    setupListView();
    setupColumnsView();
    setupGalleryView();
    setupPathBar();

    mainLayout->addWidget(m_pathBar, 0);

    setViewMode(FinderViewMode::Columns);

    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &FinderColumnView::reloadCurrentDirectory);

    connect(&m_storageTimer, &QTimer::timeout, this, &FinderColumnView::updateStorageStatus);
    m_storageTimer.start(30000);
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
    m_iconsList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_iconsList->setStyleSheet(QStringLiteral("QListWidget { background: #FFFFFF; border: none; }"));

    connect(m_iconsList, &QListWidget::itemClicked, this, &FinderColumnView::onIconItemClicked);
    connect(m_iconsList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        if (item) {
            Q_EMIT fileDoubleClicked(item->data(Qt::UserRole).toString());
        }
    });
    connect(m_iconsList, &QListWidget::itemSelectionChanged, this, &FinderColumnView::updateStorageStatus);
    m_viewStack->addWidget(m_iconsList);
}

void FinderColumnView::setupListView()
{
    m_listTree = new QTreeWidget(this);
    m_listTree->setColumnCount(3);
    m_listTree->setHeaderLabels({ QStringLiteral("Nombre"), QStringLiteral("Fecha de modificación"), QStringLiteral("Tamaño") });
    m_listTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_listTree->setIndentation(18);
    m_listTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_listTree->setStyleSheet(QStringLiteral("QTreeWidget { background: #FFFFFF; border: none; }"));
    m_listTree->setItemDelegate(new TagCircleDelegate(m_listTree));

    connect(m_listTree, &QTreeWidget::itemClicked, this, &FinderColumnView::onListItemClicked);
    connect(m_listTree, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item, int /*col*/) {
        if (item) {
            Q_EMIT fileDoubleClicked(item->data(0, Qt::UserRole).toString());
        }
    });
    connect(m_listTree, &QTreeWidget::itemSelectionChanged, this, &FinderColumnView::updateStorageStatus);
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
    m_galleryThumbnailStrip->setStyleSheet(QStringLiteral("QListWidget { background: #FFFFFF; border: 1px solid #E5E5EA; border-radius: 8px; }"));

    connect(m_galleryThumbnailStrip, &QListWidget::itemClicked, this, &FinderColumnView::onGalleryItemClicked);
    connect(m_galleryThumbnailStrip, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        if (item) {
            Q_EMIT fileDoubleClicked(item->data(Qt::UserRole).toString());
        }
    });

    m_galleryDetailsLabel = new QLabel(QStringLiteral("Metadatos y Detalles"), m_galleryWidget);
    m_galleryDetailsLabel->setFixedHeight(40);
    m_galleryDetailsLabel->setAlignment(Qt::AlignCenter);

    galleryLayout->addWidget(m_galleryMainPreview, 4);
    galleryLayout->addWidget(m_galleryThumbnailStrip, 1);
    galleryLayout->addWidget(m_galleryDetailsLabel);

    m_viewStack->addWidget(m_galleryWidget);
}

void FinderColumnView::setupPathBar()
{
    m_pathBar = new QWidget(this);
    m_pathBar->setFixedHeight(22);
    m_pathBar->setStyleSheet(QStringLiteral(
        "QWidget { background-color: #F6F6F6; border-top: 1px solid #D1D1D6; font-size: 11px; }"
        "QToolButton { border: none; background: transparent; padding: 0 4px; color: #3C3C43; font-weight: 500; }"
        "QToolButton:hover { color: #007AFF; }"
    ));

    auto *hLayout = new QHBoxLayout(m_pathBar);
    hLayout->setContentsMargins(8, 0, 8, 0);
    hLayout->setSpacing(4);

    auto *breadcrumbsContainer = new QWidget(m_pathBar);
    m_breadcrumbsLayout = new QHBoxLayout(breadcrumbsContainer);
    m_breadcrumbsLayout->setContentsMargins(0, 0, 0, 0);
    m_breadcrumbsLayout->setSpacing(2);

    m_statusLabel = new QLabel(m_pathBar);
    m_statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_statusLabel->setStyleSheet(QStringLiteral("color: #8E8E93; font-size: 11px;"));

    hLayout->addWidget(breadcrumbsContainer);
    hLayout->addStretch(1);
    hLayout->addWidget(m_statusLabel);
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

void FinderColumnView::setTagFilter(const QString &tag)
{
    if (m_tagFilter != tag) {
        m_tagFilter = tag;
        reloadCurrentDirectory();
    }
}

void FinderColumnView::reloadCurrentDirectory()
{
    if (!m_rootPath.isEmpty()) {
        setRootPath(m_rootPath);
    }
}

void FinderColumnView::setRootPath(const QString &path)
{
    m_rootPath = path;

    // Actualizar observador de cambios
    if (!m_watcher.directories().isEmpty()) {
        m_watcher.removePaths(m_watcher.directories());
    }
    if (QDir(path).exists()) {
        m_watcher.addPath(path);
    }

    truncateColumnsAfter(-1);
    addColumn(path);

    QDir dir(path);
    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);

    m_iconsList->clear();
    m_listTree->clear();
    m_galleryThumbnailStrip->clear();

    for (const QFileInfo &info : entries) {
        QString tag = FileOperations::getFileTag(info.absoluteFilePath());
        if (!m_tagFilter.isEmpty() && tag != m_tagFilter) {
            continue;
        }

        // Iconos
        auto *iconItem = new QListWidgetItem(info.fileName(), m_iconsList);
        iconItem->setData(Qt::UserRole, info.absoluteFilePath());

        // Lista con columnas y delegado para tag
        auto *treeItem = new QTreeWidgetItem(m_listTree);
        treeItem->setText(0, info.isDir() ? QStringLiteral("▶  ") + info.fileName() : info.fileName());
        treeItem->setText(1, info.lastModified().toString(QStringLiteral("dd/MM/yyyy HH:mm")));
        treeItem->setText(2, info.isDir() ? QStringLiteral("--") : QString::number(info.size() / 1024) + QStringLiteral(" KB"));
        treeItem->setData(0, Qt::UserRole, info.absoluteFilePath());
        treeItem->setData(0, QFileSystemModel::FilePathRole, info.absoluteFilePath());

        // Galería
        auto *galleryItem = new QListWidgetItem(info.fileName(), m_galleryThumbnailStrip);
        galleryItem->setData(Qt::UserRole, info.absoluteFilePath());
        galleryItem->setData(Qt::UserRole + 1, info.isDir());
    }

    if (m_galleryThumbnailStrip->count() > 0) {
        m_galleryThumbnailStrip->setCurrentRow(0);
        onGalleryItemClicked(m_galleryThumbnailStrip->item(0));
    } else {
        m_galleryMainPreview->clear();
        m_galleryMainPreview->setText(QStringLiteral("Carpeta vacía"));
        m_galleryDetailsLabel->setText(QString());
    }

    updatePathBar();
    updateStorageStatus();
}

void FinderColumnView::updatePathBar()
{
    if (!m_breadcrumbsLayout) {
        return;
    }

    // Limpiar breadcrumbs anteriores
    QLayoutItem *child;
    while ((child = m_breadcrumbsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    QString normalized = QDir::cleanPath(m_rootPath);
    QStringList parts = normalized.split(QLatin1Char('/'), Qt::SkipEmptyParts);

    QString currentAcc = QStringLiteral("/");
    auto *rootBtn = new QToolButton(m_pathBar);
    rootBtn->setText(QStringLiteral("Macintosh HD"));
    rootBtn->setAutoRaise(true);
    connect(rootBtn, &QToolButton::clicked, this, [this]() {
        Q_EMIT pathNavigated(QStringLiteral("/"));
    });
    m_breadcrumbsLayout->addWidget(rootBtn);

    for (int i = 0; i < parts.size(); ++i) {
        auto *sep = new QLabel(QStringLiteral("›"), m_pathBar);
        sep->setStyleSheet(QStringLiteral("color: #C7C7CC; font-size: 12px;"));
        m_breadcrumbsLayout->addWidget(sep);

        currentAcc += parts[i];
        QString stepPath = currentAcc;
        if (i < parts.size() - 1) {
            currentAcc += QStringLiteral("/");
        }

        auto *btn = new QToolButton(m_pathBar);
        btn->setText(parts[i]);
        btn->setAutoRaise(true);
        connect(btn, &QToolButton::clicked, this, [this, stepPath]() {
            Q_EMIT pathNavigated(stepPath);
        });
        m_breadcrumbsLayout->addWidget(btn);
    }
}

void FinderColumnView::updateStorageStatus()
{
    if (!m_statusLabel) {
        return;
    }

    int totalItems = 0;
    int selectedCount = 0;

    switch (m_viewMode) {
    case FinderViewMode::Icons:
        totalItems = m_iconsList->count();
        selectedCount = m_iconsList->selectedItems().count();
        break;
    case FinderViewMode::List:
        totalItems = m_listTree->topLevelItemCount();
        selectedCount = m_listTree->selectedItems().count();
        break;
    case FinderViewMode::Columns:
        if (!m_columns.isEmpty() && m_columns.last()) {
            totalItems = m_columns.last()->count();
            selectedCount = m_columns.last()->selectedItems().count();
        }
        break;
    case FinderViewMode::Gallery:
        totalItems = m_galleryThumbnailStrip->count();
        selectedCount = m_galleryThumbnailStrip->selectedItems().count();
        break;
    }

    QStorageInfo storage(m_rootPath.isEmpty() ? QDir::homePath() : m_rootPath);
    qint64 freeBytes = storage.bytesAvailable();
    double freeGB = static_cast<double>(freeBytes) / (1024.0 * 1024.0 * 1024.0);

    if (selectedCount > 0) {
        m_statusLabel->setText(QStringLiteral("%1 de %2 seleccionados, %3 GB disponibles")
                                   .arg(selectedCount)
                                   .arg(totalItems)
                                   .arg(freeGB, 0, 'f', 1));
    } else {
        m_statusLabel->setText(QStringLiteral("%1 elementos, %2 GB disponibles")
                                   .arg(totalItems)
                                   .arg(freeGB, 0, 'f', 1));
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
    listWidget->setItemDelegate(new TagCircleDelegate(listWidget));
    listWidget->setStyleSheet(QStringLiteral(
        "QListWidget { background-color: #FFFFFF; border-right: 1px solid #E5E5EA; }"
        "QListWidget::item { height: 26px; padding-left: 6px; border-radius: 4px; }"
        "QListWidget::item:selected { background-color: #007AFF; color: white; }"
    ));

    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
    for (const QFileInfo &info : entries) {
        if (!m_tagFilter.isEmpty()) {
            QString tag = FileOperations::getFileTag(info.absoluteFilePath());
            if (tag != m_tagFilter) {
                continue;
            }
        }

        QString displayName = info.fileName();
        if (info.isDir()) {
            displayName += QStringLiteral("  ›");
        }
        auto *item = new QListWidgetItem(displayName, listWidget);
        item->setData(Qt::UserRole, info.absoluteFilePath());
        item->setData(Qt::UserRole + 1, info.isDir());
        item->setData(QFileSystemModel::FilePathRole, info.absoluteFilePath());
    }

    connect(listWidget, &QListWidget::itemClicked, this, &FinderColumnView::onColumnItemClicked);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        if (item) {
            Q_EMIT fileDoubleClicked(item->data(Qt::UserRole).toString());
        }
    });

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
    QString tag = FileOperations::getFileTag(filePath);
    QString tagStr = tag.isEmpty() ? QStringLiteral("Sin etiqueta") : tag;

    QString details = QStringLiteral("<b>Nombre:</b> %1<br>"
                                     "<b>Tipo:</b> %2<br>"
                                     "<b>Tamaño:</b> %3 bytes<br>"
                                     "<b>Etiqueta:</b> %4<br>"
                                     "<b>Modificado:</b> %5")
                          .arg(info.fileName())
                          .arg(info.isDir() ? QStringLiteral("Carpeta") : info.suffix().toUpper())
                          .arg(info.size())
                          .arg(tagStr)
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
    updateStorageStatus();
}

void FinderColumnView::onListItemClicked(QTreeWidgetItem *item, int /*column*/)
{
    QString path = item->data(0, Qt::UserRole).toString();
    m_currentSelectedFile = path;
    Q_EMIT fileSelected(path);
    updateStorageStatus();
}

void FinderColumnView::onIconItemClicked(QListWidgetItem *item)
{
    QString path = item->data(Qt::UserRole).toString();
    m_currentSelectedFile = path;
    Q_EMIT fileSelected(path);
    updateStorageStatus();
}

void FinderColumnView::onGalleryItemClicked(QListWidgetItem *item)
{
    if (!item) {
        return;
    }
    QString path = item->data(Qt::UserRole).toString();
    m_currentSelectedFile = path;
    Q_EMIT fileSelected(path);
    updateGalleryPreview(path);
    updateStorageStatus();
}

void FinderColumnView::updateGalleryPreview(const QString &filePath)
{
    QFileInfo info(filePath);
    if (!info.exists()) {
        m_galleryMainPreview->clear();
        m_galleryMainPreview->setText(QStringLiteral("Archivo no disponible"));
        m_galleryDetailsLabel->setText(QString());
        return;
    }

    QString ext = info.suffix().toLower();
    bool isImage = (ext == QLatin1String("png") || ext == QLatin1String("jpg") ||
                    ext == QLatin1String("jpeg") || ext == QLatin1String("svg") ||
                    ext == QLatin1String("webp") || ext == QLatin1String("bmp"));

    if (isImage) {
        QPixmap cached;
        if (QPixmapCache::find(filePath, &cached)) {
            m_galleryMainPreview->setPixmap(cached);
        } else {
            m_galleryMainPreview->setText(QStringLiteral("Cargando vista previa..."));
            QSize targetSize = m_galleryMainPreview->size();
            if (targetSize.width() < 100 || targetSize.height() < 100) {
                targetSize = QSize(400, 300);
            }
            (void)QtConcurrent::run([this, filePath, targetSize]() {
                QImageReader reader(filePath);
                reader.setScaledSize(reader.size().scaled(targetSize, Qt::KeepAspectRatio));
                QImage img = reader.read();
                if (!img.isNull()) {
                    QPixmap pm = QPixmap::fromImage(img);
                    QPixmapCache::insert(filePath, pm);
                    QMetaObject::invokeMethod(this, [this, filePath, pm]() {
                        if (m_currentSelectedFile == filePath) {
                            m_galleryMainPreview->setPixmap(pm);
                        }
                    });
                }
            });
        }
    } else if (info.isDir()) {
        m_galleryMainPreview->clear();
        m_galleryMainPreview->setText(QStringLiteral("📁  %1\n(Carpeta)").arg(info.fileName()));
    } else {
        m_galleryMainPreview->clear();
        m_galleryMainPreview->setText(QStringLiteral("📄  %1\n(%2)").arg(info.fileName(), info.suffix().toUpper()));
    }

    QString tag = FileOperations::getFileTag(filePath);
    QString tagLabel = tag.isEmpty() ? QStringLiteral("Sin etiqueta") : tag;

    QString details = QStringLiteral("<b>%1</b> &nbsp;|&nbsp; %2 &nbsp;|&nbsp; %3 &nbsp;|&nbsp; %4 &nbsp;|&nbsp; %5")
                          .arg(info.fileName())
                          .arg(info.isDir() ? QStringLiteral("Carpeta") : QString::number(info.size() / 1024) + QStringLiteral(" KB"))
                          .arg(info.isDir() ? QStringLiteral("Directorio") : (info.suffix().isEmpty() ? QStringLiteral("Archivo") : info.suffix().toUpper()))
                          .arg(tagLabel)
                          .arg(info.lastModified().toString(QStringLiteral("dd/MM/yyyy HH:mm")));

    m_galleryDetailsLabel->setText(details);
}

QList<QString> FinderColumnView::selectedFilePaths() const
{
    QList<QString> paths;
    switch (m_viewMode) {
    case FinderViewMode::Icons:
        for (auto *it : m_iconsList->selectedItems()) {
            paths.append(it->data(Qt::UserRole).toString());
        }
        break;
    case FinderViewMode::List:
        for (auto *it : m_listTree->selectedItems()) {
            paths.append(it->data(0, Qt::UserRole).toString());
        }
        break;
    case FinderViewMode::Columns:
        if (!m_columns.isEmpty() && m_columns.last()) {
            for (auto *it : m_columns.last()->selectedItems()) {
                paths.append(it->data(Qt::UserRole).toString());
            }
        }
        break;
    case FinderViewMode::Gallery:
        for (auto *it : m_galleryThumbnailStrip->selectedItems()) {
            paths.append(it->data(Qt::UserRole).toString());
        }
        break;
    }

    if (paths.isEmpty() && !m_currentSelectedFile.isEmpty()) {
        paths.append(m_currentSelectedFile);
    }
    return paths;
}

int FinderColumnView::galleryItemCount() const
{
    return m_galleryThumbnailStrip ? m_galleryThumbnailStrip->count() : 0;
}

int FinderColumnView::columnCount() const
{
    return m_columns.size();
}

int FinderColumnView::iconItemCount() const
{
    return m_iconsList ? m_iconsList->count() : 0;
}

int FinderColumnView::listItemCount() const
{
    return m_listTree ? m_listTree->topLevelItemCount() : 0;
}

} // namespace Quartz::Finder
