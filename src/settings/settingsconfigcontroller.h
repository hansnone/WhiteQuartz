#pragma once

#include <QObject>
#include <QString>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>

namespace Quartz::Settings {

class SettingsConfigController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool stageManagerEnabled READ stageManagerEnabled WRITE setStageManagerEnabled NOTIFY stageManagerEnabledChanged)
    Q_PROPERTY(QString appearanceMode READ appearanceMode WRITE setAppearanceMode NOTIFY appearanceModeChanged)
    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY themeNameChanged)
    Q_PROPERTY(QString iconTheme READ iconTheme WRITE setIconTheme NOTIFY iconThemeChanged)

public:
    explicit SettingsConfigController(QObject *parent = nullptr);

    bool stageManagerEnabled() const { return m_stageManagerEnabled; }
    void setStageManagerEnabled(bool enabled);

    QString appearanceMode() const { return m_appearanceMode; }
    void setAppearanceMode(const QString &mode);

    QString themeName() const { return m_themeName; }
    void setThemeName(const QString &name);

    QString iconTheme() const { return m_iconTheme; }
    void setIconTheme(const QString &theme);

public Q_SLOTS:
    void reloadConfig();
    void onConfigChanged(const QString &json);

Q_SIGNALS:
    void stageManagerEnabledChanged(bool enabled);
    void appearanceModeChanged(const QString &mode);
    void themeNameChanged(const QString &name);
    void iconThemeChanged(const QString &theme);

private:
    QDBusInterface *m_dbus = nullptr;
    bool m_stageManagerEnabled = false;
    QString m_appearanceMode = QStringLiteral("Auto");
    QString m_themeName = QStringLiteral("Aqua");
    QString m_iconTheme = QStringLiteral("Colloid");
};

} // namespace Quartz::Settings
