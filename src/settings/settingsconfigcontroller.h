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
    Q_PROPERTY(QString wallpaper READ wallpaper WRITE setWallpaper NOTIFY wallpaperChanged)
    Q_PROPERTY(int dockSize READ dockSize WRITE setDockSize NOTIFY dockSizeChanged)
    Q_PROPERTY(bool magnificationEnabled READ magnificationEnabled WRITE setMagnificationEnabled NOTIFY magnificationEnabledChanged)

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

    QString wallpaper() const { return m_wallpaper; }
    void setWallpaper(const QString &wp);

    int dockSize() const { return m_dockSize; }
    void setDockSize(int size);

    bool magnificationEnabled() const { return m_magnificationEnabled; }
    void setMagnificationEnabled(bool enabled);

public Q_SLOTS:
    void reloadConfig();
    void onConfigChanged(const QString &json);

Q_SIGNALS:
    void stageManagerEnabledChanged(bool enabled);
    void appearanceModeChanged(const QString &mode);
    void themeNameChanged(const QString &name);
    void iconThemeChanged(const QString &theme);
    void wallpaperChanged(const QString &wallpaper);
    void dockSizeChanged(int size);
    void magnificationEnabledChanged(bool enabled);

private:
    QDBusInterface *m_dbus = nullptr;
    bool m_stageManagerEnabled = false;
    QString m_appearanceMode = QStringLiteral("Auto");
    QString m_themeName = QStringLiteral("Aqua");
    QString m_iconTheme = QStringLiteral("Colloid");
    QString m_wallpaper = QStringLiteral("file:///usr/share/backgrounds/quartz.png");
    int m_dockSize = 48;
    bool m_magnificationEnabled = true;
};

} // namespace Quartz::Settings
