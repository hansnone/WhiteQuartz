#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

namespace Quartz::Shell {

class LaunchpadController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)
    Q_PROPERTY(QVariantList apps READ apps NOTIFY appsChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY appsChanged)

public:
    explicit LaunchpadController(QObject *parent = nullptr);

    bool visible() const { return m_visible; }
    void setVisible(bool vis);

    QString filterText() const { return m_filterText; }
    void setFilterText(const QString &text);

    QVariantList apps() const;
    int pageCount() const;

public Q_SLOTS:
    void toggle();
    void reloadApps();
    void launchApp(const QString &exec);

Q_SIGNALS:
    void visibleChanged(bool visible);
    void filterTextChanged(const QString &text);
    void appsChanged();

private:
    bool m_visible = false;
    QString m_filterText;
    QVariantList m_allApps;
    void scanDesktopFiles();
};

} // namespace Quartz::Shell
