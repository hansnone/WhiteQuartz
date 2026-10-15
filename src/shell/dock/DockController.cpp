#include "DockController.h"
#include <QProcess>
#include <QDir>

namespace Quartz::Shell {

DockController::DockController(QObject *parent)
    : QObject(parent)
{
}

qreal DockController::calculateGaussianMagnification(qreal itemCenterDistance, qreal sigma) const
{
    if (sigma <= 0.0) {
        return static_cast<qreal>(StyleMetrics::DockBaseIconSize);
    }

    qreal d = itemCenterDistance;
    // Campana Gaussiana de magnificación: S(x) = Base + (Max - Base) * exp(-d^2 / (2 * sigma^2))
    qreal exponent = -(d * d) / (2.0 * sigma * sigma);
    qreal factor = qExp(exponent);

    qreal base = static_cast<qreal>(StyleMetrics::DockBaseIconSize);
    qreal max = static_cast<qreal>(StyleMetrics::DockMaxMagnifiedSize);

    return base + (max - base) * factor;
}

qreal DockController::calculateParabolicBounce(qreal progress, qreal bounceHeight) const
{
    // Trayectoria parabólica vertical para el rebote de lanzamiento:
    // y(t) = -4 * h * (t) * (t - 1) donde t en [0, 1]
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
    list.append(makeItem(QStringLiteral("Finder"), QStringLiteral("quartz-finder"), QStringLiteral("system-file-manager"), true));
    list.append(makeItem(QStringLiteral("Launchpad"), QStringLiteral("quartz-spotlight"), QStringLiteral("user-desktop"), false));
    list.append(makeItem(QStringLiteral("Ajustes del Sistema"), QStringLiteral("quartz-settings"), QStringLiteral("preferences-system"), false));
    list.append(makeItem(QStringLiteral("Terminal"), QStringLiteral("x-terminal-emulator"), QStringLiteral("utilities-terminal"), true));

    // Separador
    list.append(makeItem(QStringLiteral(""), QStringLiteral(""), QStringLiteral(""), false, true));

    // Stacks y Papelera
    list.append(makeItem(QStringLiteral("Descargas"), QStringLiteral("quartz-finder ~/Descargas"), QStringLiteral("folder-download"), false));
    list.append(makeItem(QStringLiteral("Papelera"), QStringLiteral("quartz-finder trash:/"), QStringLiteral("user-trash"), false));

    return list;
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
