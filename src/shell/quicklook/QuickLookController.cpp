#include "QuickLookController.h"
#include <QFile>
#include <QTextStream>
#include <QUrl>
#include <QDebug>

namespace Quartz::Shell {

QuickLookController::QuickLookController(QObject *parent)
    : QObject(parent)
{
    m_dbus = new QDBusInterface(
        QStringLiteral("org.quartz.QuickLook"),
        QStringLiteral("/org/quartz/QuickLook"),
        QStringLiteral("org.quartz.QuickLook"),
        QDBusConnection::sessionBus(),
        this
    );

    // Conectar señales de D-Bus emitidas por quartz-quicklookd
    QDBusConnection::sessionBus().connect(
        QStringLiteral("org.quartz.QuickLook"),
        QStringLiteral("/org/quartz/QuickLook"),
        QStringLiteral("org.quartz.QuickLook"),
        QStringLiteral("preview_requested"),
        this,
        SLOT(onPreviewRequested(QString, QString, QString, QString, QString))
    );

    QDBusConnection::sessionBus().connect(
        QStringLiteral("org.quartz.QuickLook"),
        QStringLiteral("/org/quartz/QuickLook"),
        QStringLiteral("org.quartz.QuickLook"),
        QStringLiteral("preview_closed"),
        this,
        SLOT(onPreviewClosed())
    );
}

void QuickLookController::setVisible(bool vis)
{
    if (m_visible != vis) {
        m_visible = vis;
        Q_EMIT visibleChanged(m_visible);
        if (!m_visible && m_dbus && m_dbus->isValid()) {
            m_dbus->call(QStringLiteral("close_preview"));
        }
    }
}

void QuickLookController::closePreview()
{
    setVisible(false);
}

void QuickLookController::onPreviewClosed()
{
    if (m_visible) {
        m_visible = false;
        Q_EMIT visibleChanged(m_visible);
    }
}

void QuickLookController::onPreviewRequested(const QString &uri, const QString &fileName, const QString &summary, const QString &previewType, const QString &textData)
{
    m_fileUri = uri;
    m_fileName = fileName;
    m_summary = summary;
    m_previewType = previewType;

    if (!textData.isEmpty()) {
        m_textContent = textData;
    } else {
        m_textContent.clear();
        QString localPath = QUrl(uri).isLocalFile() ? QUrl(uri).toLocalFile() : uri;
        if (localPath.startsWith(QStringLiteral("file://"))) {
            localPath = localPath.mid(7);
        }

        if (previewType == QStringLiteral("Text") || previewType.contains(QStringLiteral("Text"))) {
            QFile file(localPath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                QStringList lines;
                int count = 0;
                while (!in.atEnd() && count < 80) {
                    lines.append(in.readLine());
                    count++;
                }
                m_textContent = lines.join(QStringLiteral("\n"));
            }
        }
    }

    m_visible = true;
    Q_EMIT fileDetailsChanged();
    Q_EMIT visibleChanged(m_visible);
}

} // namespace Quartz::Shell
