#include "DockController.h"
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStandardPaths>
#include <QDBusConnection>

namespace Quartz::Shell {

DockController::DockController(QObject *parent)
    : QObject(parent)
{
    // Papelera: inicializar y monitorizar directorio de archivos
    QString trashPath = QDir::homePath() + QStringLiteral("/.local/share/Trash/files");
    QDir().mkpath(trashPath);
    m_trashWatcher.addPath(trashPath);

    connect(&m_trashWatcher, &QFileSystemWatcher::directoryChanged, this, &DockController::updateTrashState);
    connect(&m_trashWatcher, &QFileSystemWatcher::fileChanged, this, &DockController::updateTrashState);

    updateTrashState();
    rebuildAppList();

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (bus.isConnected()) {
        bus.registerObject(QStringLiteral("/org/quartz/Shell/Dock"), this,
                           QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals);
    }
}

qreal DockController::calculateGaussianMagnification(qreal itemCenterDistance, qreal sigma) const
{
    if (sigma <= 0.0) {
        return static_cast<qreal>(StyleMetrics::DockBaseIconSize);
    }

    qreal d = itemCenterDistance;
    qreal exponent = -(d * d) / (2.0 * sigma * sigma);
    qreal factor = qExp(exponent);

    qreal base = static_cast<qreal>(StyleMetrics::DockBaseIconSize);
    qreal max = static_cast<qreal>(StyleMetrics::DockMaxMagnifiedSize);

    return base + (max - base) * factor;
}

qreal DockController::calculateParabolicBounce(qreal progress, qreal bounceHeight) const
{
    if (progress < 0.0 || progress > 1.0) {
        return 0.0;
    }
    return -4.0 * bounceHeight * progress * (progress - 1.0);
}

QVariantList DockController::defaultApps() const
{
    QVariantList list;

    auto makeItem = [](const QString &name, const QString &cmd, const QString &icon, bool running, bool isSeparator = false) {
        QVariantMap map;
        map[QStringLiteral("name")] = name;
        map[QStringLiteral("cmd")] = cmd;
        map[QStringLiteral("icon")] = icon;
        map[QStringLiteral("running")] = running;
        map[QStringLiteral("isSeparator")] = isSeparator;
        return map;
    };

    // Aplicaciones ancladas
    list.append(makeItem(QStringLiteral("Finder"), QStringLiteral("quartz-finder"), QStringLiteral("system-file-manager"), false));
    list.append(makeItem(QStringLiteral("Launchpad"), QStringLiteral("quartz-spotlight"), QStringLiteral("user-desktop"), false));
    list.append(makeItem(QStringLiteral("Ajustes del Sistema"), QStringLiteral("quartz-settings"), QStringLiteral("preferences-system"), false));
    list.append(makeItem(QStringLiteral("Terminal"), QStringLiteral("x-terminal-emulator"), QStringLiteral("utilities-terminal"), false));

    // Separador
    list.append(makeItem(QStringLiteral(""), QStringLiteral(""), QStringLiteral(""), false, true));

    // Stacks y Papelera
    list.append(makeItem(QStringLiteral("Descargas"), QStringLiteral("quartz-finder ~/Descargas"), QStringLiteral("folder-download"), false));
    list.append(makeItem(QStringLiteral("Papelera"), QStringLiteral("quartz-finder trash:/"), m_trashIcon, false));

    return list;
}

void DockController::rebuildAppList()
{
    QVariantList base = defaultApps();
    QVariantList result;
    QSet<QString> pinnedCmds;

    // Actualizar estado 'running' para aplicaciones ancladas
    for (int i = 0; i < base.size(); ++i) {
        QVariantMap item = base.at(i).toMap();
        QString cmd = item.value(QStringLiteral("cmd")).toString();
        pinnedCmds.insert(cmd);

        if (cmd == QStringLiteral("quartz-finder")) {
            item[QStringLiteral("running")] = m_runningAppIds.contains(QStringLiteral("quartz-finder")) || m_runningAppIds.contains(QStringLiteral("Finder"));
        } else if (cmd == QStringLiteral("quartz-settings")) {
            item[QStringLiteral("running")] = m_runningAppIds.contains(QStringLiteral("quartz-settings"));
        } else if (cmd == QStringLiteral("x-terminal-emulator")) {
            item[QStringLiteral("running")] = m_runningAppIds.contains(QStringLiteral("Terminal")) || m_runningAppIds.contains(QStringLiteral("x-terminal-emulator"));
        } else if (item.value(QStringLiteral("name")).toString() == QStringLiteral("Papelera")) {
            item[QStringLiteral("icon")] = m_trashIcon;
        }

        // Antes del separador, inyectar aplicaciones en ejecución no ancladas
        if (item.value(QStringLiteral("isSeparator")).toBool()) {
            for (const QString &runningId : m_runningAppIds) {
                if (!pinnedCmds.contains(runningId) && runningId != QStringLiteral("quartz-finder") && runningId != QStringLiteral("quartz-settings")) {
                    QVariantMap unpinned;
                    unpinned[QStringLiteral("name")] = runningId;
                    unpinned[QStringLiteral("cmd")] = runningId;
                    unpinned[QStringLiteral("icon")] = QStringLiteral("application-x-executable");
                    unpinned[QStringLiteral("running")] = true;
                    unpinned[QStringLiteral("isSeparator")] = false;
                    result.append(unpinned);
                }
            }
        }

        result.append(item);
    }

    m_apps = result;
    Q_EMIT appsChanged();
}

void DockController::onWindowAdded(const QString &appId, const QString &/*title*/, const QString &/*icon*/, bool /*active*/)
{
    if (!appId.isEmpty()) {
        m_runningAppIds.insert(appId);
        rebuildAppList();
    }
}

void DockController::onWindowRemoved(const QString &appId)
{
    if (m_runningAppIds.remove(appId)) {
        rebuildAppList();
    }
}

void DockController::onActiveWindowChanged(const QString &appId, const QString &/*title*/)
{
    if (!appId.isEmpty() && !m_runningAppIds.contains(appId)) {
        m_runningAppIds.insert(appId);
        rebuildAppList();
    }
}

void DockController::updateTrashState()
{
    QString trashFiles = QDir::homePath() + QStringLiteral("/.local/share/Trash/files");
    QDir dir(trashFiles);
    bool hasFiles = false;

    if (dir.exists()) {
        hasFiles = (dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).count() > 0);
    }

    QString nextIcon = hasFiles ? QStringLiteral("user-trash-full") : QStringLiteral("user-trash");
    if (m_trashIcon != nextIcon) {
        m_trashIcon = nextIcon;
        Q_EMIT trashIconChanged(m_trashIcon);
        rebuildAppList();
    }
}

QVariantList DockController::listStackDirectory(const QString &dirPath) const
{
    QString path = dirPath;
    if (path.startsWith(QLatin1String("~/"))) {
        path = QDir::homePath() + path.mid(1);
    } else if (path == QLatin1String("~")) {
        path = QDir::homePath();
    }

    QDir dir(path);
    if (!dir.exists()) {
        return {};
    }

    QFileInfoList entries = dir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
        QDir::Time
    );

    QVariantList result;
    int limit = qMin(entries.size(), 10);

    for (int i = 0; i < limit; ++i) {
        const QFileInfo &fi = entries.at(i);
        QVariantMap item;
        item[QStringLiteral("name")] = fi.fileName();
        item[QStringLiteral("path")] = fi.absoluteFilePath();
        item[QStringLiteral("isDir")] = fi.isDir();

        if (fi.isDir()) {
            item[QStringLiteral("size")] = QStringLiteral("Carpeta");
            item[QStringLiteral("icon")] = QStringLiteral("folder");
        } else {
            qint64 bytes = fi.size();
            if (bytes < 1024) {
                item[QStringLiteral("size")] = QString::number(bytes) + QStringLiteral(" B");
            } else if (bytes < 1024 * 1024) {
                item[QStringLiteral("size")] = QString::number(bytes / 1024) + QStringLiteral(" KB");
            } else {
                item[QStringLiteral("size")] = QString::number(bytes / (1024 * 1024)) + QStringLiteral(" MB");
            }

            QString ext = fi.suffix().toLower();
            if (ext == QStringLiteral("png") || ext == QStringLiteral("jpg") || ext == QStringLiteral("jpeg") || ext == QStringLiteral("webp") || ext == QStringLiteral("svg")) {
                item[QStringLiteral("icon")] = QStringLiteral("image-x-generic");
            } else if (ext == QStringLiteral("mp3") || ext == QStringLiteral("wav") || ext == QStringLiteral("flac")) {
                item[QStringLiteral("icon")] = QStringLiteral("audio-x-generic");
            } else if (ext == QStringLiteral("mp4") || ext == QStringLiteral("mkv") || ext == QStringLiteral("webm")) {
                item[QStringLiteral("icon")] = QStringLiteral("video-x-generic");
            } else if (ext == QStringLiteral("pdf")) {
                item[QStringLiteral("icon")] = QStringLiteral("application-pdf");
            } else if (ext == QStringLiteral("zip") || ext == QStringLiteral("tar") || ext == QStringLiteral("gz") || ext == QStringLiteral("deb")) {
                item[QStringLiteral("icon")] = QStringLiteral("package-x-generic");
            } else {
                item[QStringLiteral("icon")] = QStringLiteral("text-plain");
            }
        }

        result.append(item);
    }

    return result;
}

QStringList DockController::parseCommandLine(const QString &cmd) const
{
    if (cmd.trimmed().isEmpty()) {
        return {};
    }

    QStringList parts = QProcess::splitCommand(cmd);
    for (QString &arg : parts) {
        if (arg.startsWith(QLatin1String("~/"))) {
            arg = QDir::homePath() + arg.mid(1);
        } else if (arg == QLatin1String("~")) {
            arg = QDir::homePath();
        }
    }
    return parts;
}

void DockController::launchApp(const QString &cmd)
{
    if (cmd.trimmed().isEmpty()) {
        return;
    }

    Q_EMIT appBouncingRequested(cmd);

    QStringList parts = parseCommandLine(cmd);
    if (!parts.isEmpty()) {
        QString program = parts.takeFirst();
        QProcess::startDetached(program, parts);
    }
}

void DockController::requestAppAttention(const QString &cmd)
{
    Q_EMIT appBouncingRequested(cmd);
}

} // namespace Quartz::Shell
