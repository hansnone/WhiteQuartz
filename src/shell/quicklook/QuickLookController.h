#pragma once

#include <QObject>
#include <QString>
#include <QDBusInterface>
#include <QDBusConnection>

namespace Quartz::Shell {

class QuickLookController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QString fileUri READ fileUri NOTIFY fileDetailsChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileDetailsChanged)
    Q_PROPERTY(QString summary READ summary NOTIFY fileDetailsChanged)
    Q_PROPERTY(QString previewType READ previewType NOTIFY fileDetailsChanged)
    Q_PROPERTY(QString textContent READ textContent NOTIFY fileDetailsChanged)

public:
    explicit QuickLookController(QObject *parent = nullptr);

    bool visible() const { return m_visible; }
    void setVisible(bool vis);

    QString fileUri() const { return m_fileUri; }
    QString fileName() const { return m_fileName; }
    QString summary() const { return m_summary; }
    QString previewType() const { return m_previewType; }
    QString textContent() const { return m_textContent; }

public Q_SLOTS:
    void closePreview();
    void onPreviewRequested(const QString &uri, const QString &fileName, const QString &summary, const QString &previewType);
    void onPreviewClosed();

Q_SIGNALS:
    void visibleChanged(bool visible);
    void fileDetailsChanged();

private:
    QDBusInterface *m_dbus = nullptr;
    bool m_visible = false;
    QString m_fileUri;
    QString m_fileName;
    QString m_summary;
    QString m_previewType = QStringLiteral("Generic");
    QString m_textContent;
};

} // namespace Quartz::Shell
