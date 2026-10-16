#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QDateTime>
#include <QDBusConnection>

namespace Quartz::Shell {

class TimeMachineService : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.quartz.TimeMachine")
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString lastBackupTime READ lastBackupTime NOTIFY lastBackupTimeChanged)
    Q_PROPERTY(QVariantList snapshots READ snapshots NOTIFY snapshotsChanged)

public:
    explicit TimeMachineService(QObject *parent = nullptr);

    QString status() const { return m_status; }
    int progress() const { return m_progress; }
    QString lastBackupTime() const { return m_lastBackupTime; }
    QVariantList snapshots() const { return m_snapshots; }

    bool registerService();

public Q_SLOTS:
    void startBackup();
    void stopBackup();
    void refreshSnapshots();

Q_SIGNALS:
    void statusChanged(const QString &status);
    void progressChanged(int progress);
    void lastBackupTimeChanged(const QString &time);
    void snapshotsChanged();
    void backupCompleted(bool success, const QString &message);

private:
    QString m_status = QStringLiteral("Inactivo");
    int m_progress = 0;
    QString m_lastBackupTime = QStringLiteral("Nunca");
    QVariantList m_snapshots;
};

class KeychainService : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.quartz.Keychain")

public:
    explicit KeychainService(QObject *parent = nullptr);
    bool registerService();

public Q_SLOTS:
    bool storeSecret(const QString &service, const QString &account, const QString &secret);
    QString lookupSecret(const QString &service, const QString &account);
    bool deleteSecret(const QString &service, const QString &account);
    QVariantList listKeys();

Q_SIGNALS:
    void secretChanged(const QString &service, const QString &account);

private:
    QVariantMap m_vault;
};

class PrintService : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.quartz.PrintService")
    Q_PROPERTY(QVariantList printers READ printers NOTIFY printersChanged)

public:
    explicit PrintService(QObject *parent = nullptr);
    bool registerService();

    QVariantList printers() const { return m_printers; }

public Q_SLOTS:
    void refreshPrinters();
    bool cancelJob(const QString &jobId);

Q_SIGNALS:
    void printersChanged();

private:
    QVariantList m_printers;
};

} // namespace Quartz::Shell
