#include "LaunchpadController.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QStandardPaths>
#include <QtMath>
#include <algorithm>

namespace Quartz::Shell {

LaunchpadController::LaunchpadController(QObject *parent)
    : QObject(parent)
{
    reloadApps();
}

void LaunchpadController::setVisible(bool vis)
{
    if (m_visible != vis) {
        m_visible = vis;
        if (!m_visible) {
            setFilterText(QString());
        }
        Q_EMIT visibleChanged(m_visible);
    }
}

void LaunchpadController::setFilterText(const QString &text)
{
    if (m_filterText != text) {
        m_filterText = text;
        Q_EMIT filterTextChanged(m_filterText);
        Q_EMIT appsChanged();
    }
}

void LaunchpadController::toggle()
{
    setVisible(!m_visible);
}

QVariantList LaunchpadController::apps() const
{
    if (m_filterText.trimmed().isEmpty()) {
        return m_allApps;
    }

    QVariantList filtered;
    for (const auto &var : m_allApps) {
        QVariantMap map = var.toMap();
        QString name = map.value(QStringLiteral("name")).toString();
        QString comment = map.value(QStringLiteral("comment")).toString();
        if (name.contains(m_filterText, Qt::CaseInsensitive) ||
            comment.contains(m_filterText, Qt::CaseInsensitive)) {
            filtered.append(var);
        }
    }
    return filtered;
}

int LaunchpadController::pageCount() const
{
    int total = apps().size();
    if (total == 0) return 1;
    return qCeil(static_cast<qreal>(total) / 24.0);
}

void LaunchpadController::reloadApps()
{
    m_allApps.clear();
    scanDesktopFiles();

    // Garantizar que las aplicaciones base de Quartz siempre estén presentes
    const QList<QVariantMap> coreApps = {
        { {QStringLiteral("name"), QStringLiteral("Finder")}, {QStringLiteral("exec"), QStringLiteral("quartz-finder")}, {QStringLiteral("icon"), QStringLiteral("system-file-manager")}, {QStringLiteral("comment"), QStringLiteral("Gestor de archivos")} },
        { {QStringLiteral("name"), QStringLiteral("Ajustes del Sistema")}, {QStringLiteral("exec"), QStringLiteral("quartz-settings")}, {QStringLiteral("icon"), QStringLiteral("preferences-system")}, {QStringLiteral("comment"), QStringLiteral("Preferencias del sistema")} },
        { {QStringLiteral("name"), QStringLiteral("Terminal")}, {QStringLiteral("exec"), QStringLiteral("x-terminal-emulator")}, {QStringLiteral("icon"), QStringLiteral("utilities-terminal")}, {QStringLiteral("comment"), QStringLiteral("Línea de comandos")} }
    };

    for (const auto &core : coreApps) {
        QString name = core.value(QStringLiteral("name")).toString();
        bool exists = false;
        for (const auto &app : m_allApps) {
            if (app.toMap().value(QStringLiteral("name")).toString().compare(name, Qt::CaseInsensitive) == 0) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            m_allApps.prepend(core);
        }
    }

    Q_EMIT appsChanged();
}

void LaunchpadController::scanDesktopFiles()
{
    QStringList searchDirs = {
        QStringLiteral("/usr/share/applications"),
        QStringLiteral("/usr/local/share/applications"),
        QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)
    };

    QSet<QString> seenExecs;

    for (const QString &dirPath : searchDirs) {
        QDir dir(dirPath);
        if (!dir.exists()) continue;

        const QStringList entries = dir.entryList({ QStringLiteral("*.desktop") }, QDir::Files);
        for (const QString &entry : entries) {
            QFile file(dir.absoluteFilePath(entry));
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

            QTextStream in(&file);
            bool inDesktopEntry = false;
            QString name, exec, icon, comment;
            bool noDisplay = false;

            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (line.startsWith(QLatin1Char('['))) {
                    inDesktopEntry = (line == QStringLiteral("[Desktop Entry]"));
                    continue;
                }
                if (!inDesktopEntry || line.isEmpty() || line.startsWith(QLatin1Char('#'))) continue;

                int eqIdx = line.indexOf(QLatin1Char('='));
                if (eqIdx == -1) continue;

                QString key = line.left(eqIdx).trimmed();
                QString val = line.mid(eqIdx + 1).trimmed();

                if (key == QStringLiteral("Name") && name.isEmpty()) {
                    name = val;
                } else if (key == QStringLiteral("Exec") && exec.isEmpty()) {
                    QStringList tokens = val.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                    QStringList cleanedTokens;
                    for (const QString &t : tokens) {
                        if (!t.startsWith(QLatin1Char('%'))) {
                            cleanedTokens.append(t);
                        }
                    }
                    exec = cleanedTokens.join(QLatin1Char(' '));
                } else if (key == QStringLiteral("Icon") && icon.isEmpty()) {
                    icon = val;
                } else if (key == QStringLiteral("Comment") && comment.isEmpty()) {
                    comment = val;
                } else if (key == QStringLiteral("NoDisplay")) {
                    noDisplay = (val.compare(QStringLiteral("true"), Qt::CaseInsensitive) == 0);
                }
            }

            if (noDisplay || name.isEmpty() || exec.isEmpty()) continue;
            if (seenExecs.contains(exec)) continue;
            seenExecs.insert(exec);

            QVariantMap map;
            map[QStringLiteral("name")] = name;
            map[QStringLiteral("exec")] = exec;
            map[QStringLiteral("icon")] = icon.isEmpty() ? QStringLiteral("application-x-executable") : icon;
            map[QStringLiteral("comment")] = comment;
            m_allApps.append(map);
        }
    }

    std::sort(m_allApps.begin(), m_allApps.end(), [](const QVariant &a, const QVariant &b) {
        return a.toMap().value(QStringLiteral("name")).toString().localeAwareCompare(
            b.toMap().value(QStringLiteral("name")).toString()) < 0;
    });
}

void LaunchpadController::launchApp(const QString &exec)
{
    if (exec.trimmed().isEmpty()) return;
    setVisible(false);

    QStringList parts = QProcess::splitCommand(exec);
    if (!parts.isEmpty()) {
        QString program = parts.takeFirst();
        QProcess::startDetached(program, parts);
    }
}

} // namespace Quartz::Shell
