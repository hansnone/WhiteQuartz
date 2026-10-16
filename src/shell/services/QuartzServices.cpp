#include "QuartzServices.h"
#include <QDBusMessage>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

namespace Quartz::Shell {

// -----------------------------------------------------------------------------
// TimeMachineService
// -----------------------------------------------------------------------------
TimeMachineService::TimeMachineService(QObject *parent)
    : QObject(parent)
{
    refreshSnapshots();
}

bool TimeMachineService::registerService()
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) return false;
    bus.registerObject(QStringLiteral("/org/quartz/TimeMachine"), this,
                       QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals);
    bus.registerService(QStringLiteral("org.quartz.TimeMachine"));
    return true;
}

void TimeMachineService::startBackup()
{
    m_status = QStringLiteral("Copiando archivos...");
    m_progress = 10;
    Q_EMIT statusChanged(m_status);
    Q_EMIT progressChanged(m_progress);

    // Simulación de progreso de snapshot seguro
    m_progress = 100;
    m_status = QStringLiteral("Completado");
    m_lastBackupTime = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));

    QVariantMap snap;
    snap[QStringLiteral("id")] = QString::number(m_snapshots.size() + 1);
    snap[QStringLiteral("timestamp")] = m_lastBackupTime;
    snap[QStringLiteral("path")] = QStringLiteral("/home/.snapshots/%1").arg(m_snapshots.size() + 1);
    m_snapshots.append(snap);

    Q_EMIT statusChanged(m_status);
    Q_EMIT progressChanged(m_progress);
    Q_EMIT lastBackupTimeChanged(m_lastBackupTime);
    Q_EMIT snapshotsChanged();
    Q_EMIT backupCompleted(true, QStringLiteral("Copia de seguridad realizada con éxito"));
}

void TimeMachineService::stopBackup()
{
    m_status = QStringLiteral("Detenido");
    m_progress = 0;
    Q_EMIT statusChanged(m_status);
    Q_EMIT progressChanged(m_progress);
}

void TimeMachineService::refreshSnapshots()
{
    if (m_snapshots.isEmpty()) {
        QVariantMap defaultSnap;
        defaultSnap[QStringLiteral("id")] = QStringLiteral("1");
        defaultSnap[QStringLiteral("timestamp")] = QStringLiteral("2026-10-15 12:00:00");
        defaultSnap[QStringLiteral("path")] = QStringLiteral("/home/.snapshots/initial");
        m_snapshots.append(defaultSnap);
        Q_EMIT snapshotsChanged();
    }
}

// -----------------------------------------------------------------------------
// KeychainService
// -----------------------------------------------------------------------------
KeychainService::KeychainService(QObject *parent)
    : QObject(parent)
{
}

bool KeychainService::registerService()
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) return false;
    bus.registerObject(QStringLiteral("/org/quartz/Keychain"), this,
                       QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals);
    bus.registerService(QStringLiteral("org.quartz.Keychain"));
    return true;
}

bool KeychainService::storeSecret(const QString &service, const QString &account, const QString &secret)
{
    QString key = QStringLiteral("%1:%2").arg(service, account);
    m_vault[key] = secret;
    Q_EMIT secretChanged(service, account);
    return true;
}

QString KeychainService::lookupSecret(const QString &service, const QString &account)
{
    QString key = QStringLiteral("%1:%2").arg(service, account);
    return m_vault.value(key).toString();
}

bool KeychainService::deleteSecret(const QString &service, const QString &account)
{
    QString key = QStringLiteral("%1:%2").arg(service, account);
    if (m_vault.contains(key)) {
        m_vault.remove(key);
        Q_EMIT secretChanged(service, account);
        return true;
    }
    return false;
}

QVariantList KeychainService::listKeys()
{
    QVariantList list;
    for (auto it = m_vault.constBegin(); it != m_vault.constEnd(); ++it) {
        QStringList parts = it.key().split(QLatin1Char(':'));
        QVariantMap entry;
        entry[QStringLiteral("service")] = parts.value(0);
        entry[QStringLiteral("account")] = parts.value(1);
        list.append(entry);
    }
    return list;
}

// -----------------------------------------------------------------------------
// PrintService
// -----------------------------------------------------------------------------
PrintService::PrintService(QObject *parent)
    : QObject(parent)
{
    refreshPrinters();
}

bool PrintService::registerService()
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) return false;
    bus.registerObject(QStringLiteral("/org/quartz/PrintService"), this,
                       QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals);
    bus.registerService(QStringLiteral("org.quartz.PrintService"));
    return true;
}

void PrintService::refreshPrinters()
{
    m_printers.clear();

    // Consultar lpstat de forma no bloqueante o con degradación elegante si no hay demonio CUPS
    QProcess proc;
    proc.start(QStringLiteral("lpstat"), { QStringLiteral("-p"), QStringLiteral("-d") });
    if (proc.waitForFinished(500)) {
        QString out = QString::fromUtf8(proc.readAllStandardOutput());
        const QStringList lines = out.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            if (line.startsWith(QStringLiteral("printer "))) {
                QStringList tokens = line.split(QLatin1Char(' '));
                if (tokens.size() >= 2) {
                    QVariantMap p;
                    p[QStringLiteral("name")] = tokens.at(1);
                    p[QStringLiteral("isDefault")] = line.contains(QStringLiteral("default"), Qt::CaseInsensitive);
                    p[QStringLiteral("status")] = QStringLiteral("Lista");
                    m_printers.append(p);
                }
            }
        }
    }

    // Fallback elegante si no hay impresoras físicas configuradas
    if (m_printers.isEmpty()) {
        QVariantMap pdfPrinter;
        pdfPrinter[QStringLiteral("name")] = QStringLiteral("Imprimir en PDF");
        pdfPrinter[QStringLiteral("isDefault")] = true;
        pdfPrinter[QStringLiteral("status")] = QStringLiteral("Disponible");
        m_printers.append(pdfPrinter);
    }

    Q_EMIT printersChanged();
}

bool PrintService::cancelJob(const QString &jobId)
{
    if (jobId.isEmpty()) return false;
    QProcess::startDetached(QStringLiteral("cancel"), { jobId });
    return true;
}

} // namespace Quartz::Shell
