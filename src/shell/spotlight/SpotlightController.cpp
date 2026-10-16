#include "SpotlightController.h"
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusArgument>
#include <QProcess>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

namespace Quartz::Shell {

SpotlightController::SpotlightController(QObject *parent)
    : QObject(parent)
{
    m_dbus = new QDBusInterface(
        QStringLiteral("org.quartz.Spotlight"),
        QStringLiteral("/org/quartz/Spotlight"),
        QStringLiteral("org.quartz.Spotlight"),
        QDBusConnection::sessionBus(),
        this
    );

    // Conectar a señal toggled
    QDBusConnection::sessionBus().connect(
        QStringLiteral("org.quartz.Spotlight"),
        QStringLiteral("/org/quartz/Spotlight"),
        QStringLiteral("org.quartz.Spotlight"),
        QStringLiteral("toggled"),
        this,
        SLOT(onToggled())
    );
}

void SpotlightController::setVisible(bool vis)
{
    if (m_visible != vis) {
        m_visible = vis;
        if (!m_visible) {
            setQueryText(QString());
            m_results.clear();
            Q_EMIT resultsChanged();
        }
        Q_EMIT visibleChanged(m_visible);
    }
}

void SpotlightController::setQueryText(const QString &text)
{
    if (m_queryText != text) {
        m_queryText = text;
        Q_EMIT queryTextChanged(m_queryText);
        search(m_queryText);
    }
}

void SpotlightController::toggle()
{
    setVisible(!m_visible);
}

void SpotlightController::onToggled()
{
    setVisible(!m_visible);
}

void SpotlightController::search(const QString &query)
{
    m_results.clear();
    QString trimmed = query.trimmed();
    if (trimmed.isEmpty()) {
        Q_EMIT resultsChanged();
        return;
    }

    if (m_dbus && m_dbus->isValid()) {
        QDBusMessage msg = m_dbus->call(QStringLiteral("search"), trimmed);
        if (msg.type() == QDBusMessage::ReplyMessage && !msg.arguments().isEmpty()) {
            const QDBusArgument &arg = msg.arguments().at(0).value<QDBusArgument>();
            if (arg.currentType() == QDBusArgument::ArrayType) {
                arg.beginArray();
                while (!arg.atEnd()) {
                    arg.beginStructure();
                    QString title;
                    QString subtitle;
                    QString category;
                    QString iconName;
                    QString payload;
                    arg >> title >> subtitle >> category >> iconName >> payload;
                    arg.endStructure();

                    QVariantMap item;
                    item[QStringLiteral("title")] = title;
                    item[QStringLiteral("subtitle")] = subtitle;
                    item[QStringLiteral("category")] = category;
                    item[QStringLiteral("icon_name")] = iconName;
                    item[QStringLiteral("payload")] = payload;
                    m_results.append(item);
                }
                arg.endArray();
                Q_EMIT resultsChanged();
                return;
            }
        }
    }

    // Fallback local si el demonio D-Bus no está activo
    if (trimmed.contains(QStringLiteral("+")) || trimmed.contains(QStringLiteral("-")) ||
        trimmed.contains(QStringLiteral("*")) || trimmed.contains(QStringLiteral("/"))) {
        QVariantMap calcItem;
        calcItem[QStringLiteral("title")] = QStringLiteral("Cálculo");
        calcItem[QStringLiteral("subtitle")] = trimmed;
        calcItem[QStringLiteral("category")] = QStringLiteral("Calculation");
        calcItem[QStringLiteral("icon_name")] = QStringLiteral("accessories-calculator");
        calcItem[QStringLiteral("payload")] = trimmed;
        m_results.append(calcItem);
    }

    const QList<QPair<QString, QString>> knownApps = {
        { QStringLiteral("Ajustes del Sistema"), QStringLiteral("quartz-settings") },
        { QStringLiteral("Finder"), QStringLiteral("quartz-finder") },
        { QStringLiteral("Terminal"), QStringLiteral("x-terminal-emulator") }
    };

    for (const auto &app : knownApps) {
        if (app.first.contains(trimmed, Qt::CaseInsensitive)) {
            QVariantMap appItem;
            appItem[QStringLiteral("title")] = app.first;
            appItem[QStringLiteral("subtitle")] = QStringLiteral("Aplicación del sistema");
            appItem[QStringLiteral("category")] = QStringLiteral("App");
            appItem[QStringLiteral("icon_name")] = QStringLiteral("application-x-executable");
            appItem[QStringLiteral("payload")] = app.second;
            m_results.append(appItem);
        }
    }

    Q_EMIT resultsChanged();
}

void SpotlightController::launchPayload(const QString &payload)
{
    if (payload.trimmed().isEmpty()) {
        return;
    }
    setVisible(false);

    QFileInfo fi(payload);
    if (fi.exists()) {
        if (fi.isDir()) {
            QProcess::startDetached(QStringLiteral("quartz-finder"), { fi.canonicalFilePath() });
        } else {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fi.canonicalFilePath()));
        }
        return;
    }

    QStringList parts = QProcess::splitCommand(payload);
    if (!parts.isEmpty()) {
        QString program = parts.takeFirst();
        QProcess::startDetached(program, parts);
    }
}

} // namespace Quartz::Shell
