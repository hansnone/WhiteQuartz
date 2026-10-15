#pragma once

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>
#include <QString>
#include <QList>

namespace Quartz::Settings {

enum class CategoryGroup : uint8_t {
    ProfileHeader = 0,
    Connectivity,
    NotificationsAndFocus,
    SystemCore,
    DisplayAndDesktop,
    SecurityAndInput
};

struct SettingsPaneItem {
    QString id;
    QString title;
    QString iconName;
    CategoryGroup group;
    QString qmlComponentPath;
    QStringList keywords;
};

class SettingsNavigationModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        IconRole,
        GroupRole,
        QmlPathRole
    };

    explicit SettingsNavigationModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void setFilterQuery(const QString &query);
    Q_INVOKABLE int totalItemsCount() const { return m_allItems.size(); }
    Q_INVOKABLE QString firstMatchingPaneId() const;

private:
    void populateItems();

    QList<SettingsPaneItem> m_allItems;
    QList<SettingsPaneItem> m_visibleItems;
};

} // namespace Quartz::Settings
