#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QDBusInterface>
#include <QDBusConnection>

namespace Quartz::Shell {

class SpotlightController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QString queryText READ queryText WRITE setQueryText NOTIFY queryTextChanged)
    Q_PROPERTY(QVariantList results READ results NOTIFY resultsChanged)

public:
    explicit SpotlightController(QObject *parent = nullptr);

    bool visible() const { return m_visible; }
    void setVisible(bool vis);

    QString queryText() const { return m_queryText; }
    void setQueryText(const QString &text);

    QVariantList results() const { return m_results; }

public Q_SLOTS:
    void toggle();
    void search(const QString &query);
    void launchPayload(const QString &payload);
    void onToggled();

Q_SIGNALS:
    void visibleChanged(bool visible);
    void queryTextChanged(const QString &text);
    void resultsChanged();

private:
    QDBusInterface *m_dbus = nullptr;
    bool m_visible = false;
    QString m_queryText;
    QVariantList m_results;
};

} // namespace Quartz::Shell
