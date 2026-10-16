#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QFileInfo>
#include <QDir>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QToolButton>

namespace Quartz::Finder {

enum class FinderViewMode {
    Icons = 1,   // Ctrl+1
    List,        // Ctrl+2
    Columns,     // Ctrl+3
    Gallery      // Ctrl+4
};

class FinderColumnView : public QWidget {
    Q_OBJECT

public:
    explicit FinderColumnView(QWidget *parent = nullptr);
    ~FinderColumnView() override = default;

    void setRootPath(const QString &path);
    QString rootPath() const { return m_rootPath; }
    QString selectedFilePath() const { return m_currentSelectedFile; }
    QList<QString> selectedFilePaths() const;

    FinderViewMode currentViewMode() const { return m_viewMode; }
    void setViewMode(FinderViewMode mode);

    void setTagFilter(const QString &tag);
    QString tagFilter() const { return m_tagFilter; }
    void reloadCurrentDirectory();

    int galleryItemCount() const;
    int columnCount() const;
    int iconItemCount() const;
    int listItemCount() const;

Q_SIGNALS:
    void fileSelected(const QString &filePath);
    void fileDoubleClicked(const QString &filePath);
    void viewModeChanged(FinderViewMode mode);
    void pathNavigated(const QString &newPath);

private Q_SLOTS:
    void onColumnItemClicked(QListWidgetItem *item);
    void onListItemClicked(QTreeWidgetItem *item, int column);
    void onIconItemClicked(QListWidgetItem *item);
    void onGalleryItemClicked(QListWidgetItem *item);
    void updateStorageStatus();

private:
    void setupIconsView();
    void setupListView();
    void setupColumnsView();
    void setupGalleryView();
    void setupPathBar();

    void addColumn(const QString &path);
    void truncateColumnsAfter(int columnIndex);
    void updateInspector(const QString &filePath);
    void updateGalleryPreview(const QString &filePath);
    void updatePathBar();

    FinderViewMode m_viewMode = FinderViewMode::Columns;
    QString m_rootPath;
    QString m_currentSelectedFile;
    QString m_tagFilter;

    QStackedWidget *m_viewStack = nullptr;

    // Barra de ruta inferior fija (22px)
    QWidget *m_pathBar = nullptr;
    QHBoxLayout *m_breadcrumbsLayout = nullptr;
    QLabel *m_statusLabel = nullptr;
    QTimer m_storageTimer;

    // Observador de cambios en el sistema de archivos
    QFileSystemWatcher m_watcher;

    // Vista de Columnas (Miller Columns)
    QScrollArea *m_columnsScroll = nullptr;
    QWidget *m_columnsContainer = nullptr;
    QHBoxLayout *m_columnsLayout = nullptr;
    QList<QListWidget *> m_columns;
    QList<QString> m_columnPaths;
    QWidget *m_inspectorWidget = nullptr;
    QLabel *m_inspectorPreviewLabel = nullptr;
    QLabel *m_inspectorDetailsLabel = nullptr;

    // Vista de Iconos
    QListWidget *m_iconsList = nullptr;

    // Vista de Lista con Triángulos de Despliegue
    QTreeWidget *m_listTree = nullptr;

    // Vista de Galería
    QWidget *m_galleryWidget = nullptr;
    QLabel *m_galleryMainPreview = nullptr;
    QListWidget *m_galleryThumbnailStrip = nullptr;
    QLabel *m_galleryDetailsLabel = nullptr;
};

} // namespace Quartz::Finder
