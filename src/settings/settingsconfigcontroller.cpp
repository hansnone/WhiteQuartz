#include "settingsconfigcontroller.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace Quartz::Settings {

SettingsConfigController::SettingsConfigController(QObject *parent)
    : QObject(parent)
{
    m_dbus = new QDBusInterface(
        QStringLiteral("org.quartz.Config"),
        QStringLiteral("/org/quartz/Config"),
        QStringLiteral("org.quartz.Config"),
        QDBusConnection::sessionBus(),
        this
    );

    if (m_dbus->isValid()) {
        reloadConfig();
        QDBusConnection::sessionBus().connect(
            QStringLiteral("org.quartz.Config"),
            QStringLiteral("/org/quartz/Config"),
            QStringLiteral("org.quartz.Config"),
            QStringLiteral("config_changed"),
            this,
            SLOT(onConfigChanged(QString))
        );
    } else {
        qWarning() << "Servicio D-Bus org.quartz.Config no disponible. Utilizando valores locales.";
    }
}

void SettingsConfigController::reloadConfig()
{
    if (!m_dbus || !m_dbus->isValid()) {
        return;
    }

    QDBusReply<QString> reply = m_dbus->call(QStringLiteral("get_config"));
    if (reply.isValid()) {
        onConfigChanged(reply.value());
    }
}

void SettingsConfigController::onConfigChanged(const QString &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isObject()) {
        return;
    }
    QJsonObject obj = doc.object();

    if (obj.contains(QStringLiteral("stage_manager"))) {
        QJsonObject sm = obj[QStringLiteral("stage_manager")].toObject();
        bool smEnabled = sm[QStringLiteral("enabled")].toBool();
        if (m_stageManagerEnabled != smEnabled) {
            m_stageManagerEnabled = smEnabled;
            Q_EMIT stageManagerEnabledChanged(m_stageManagerEnabled);
        }
    }

    if (obj.contains(QStringLiteral("appearance"))) {
        QString appMode = obj[QStringLiteral("appearance")].toString();
        if (m_appearanceMode != appMode) {
            m_appearanceMode = appMode;
            Q_EMIT appearanceModeChanged(m_appearanceMode);
        }
    }

    if (obj.contains(QStringLiteral("theme_name"))) {
        QString theme = obj[QStringLiteral("theme_name")].toString();
        if (m_themeName != theme) {
            m_themeName = theme;
            Q_EMIT themeNameChanged(m_themeName);
        }
    }

    if (obj.contains(QStringLiteral("icon_theme"))) {
        QString iconTheme = obj[QStringLiteral("icon_theme")].toString();
        if (m_iconTheme != iconTheme) {
            m_iconTheme = iconTheme;
            Q_EMIT iconThemeChanged(m_iconTheme);
        }
    }

    if (obj.contains(QStringLiteral("wallpaper"))) {
        QString wp = obj[QStringLiteral("wallpaper")].toString();
        if (m_wallpaper != wp) {
            m_wallpaper = wp;
            Q_EMIT wallpaperChanged(m_wallpaper);
        }
    }

    if (obj.contains(QStringLiteral("dock"))) {
        QJsonObject dockObj = obj[QStringLiteral("dock")].toObject();
        if (dockObj.contains(QStringLiteral("size"))) {
            int dSize = dockObj[QStringLiteral("size")].toInt();
            if (m_dockSize != dSize) {
                m_dockSize = dSize;
                Q_EMIT dockSizeChanged(m_dockSize);
            }
        }
        if (dockObj.contains(QStringLiteral("magnification"))) {
            bool mag = dockObj[QStringLiteral("magnification")].toBool();
            if (m_magnificationEnabled != mag) {
                m_magnificationEnabled = mag;
                Q_EMIT magnificationEnabledChanged(m_magnificationEnabled);
            }
        }
    }
}

void SettingsConfigController::setStageManagerEnabled(bool enabled)
{
    if (m_stageManagerEnabled != enabled) {
        m_stageManagerEnabled = enabled;
        Q_EMIT stageManagerEnabledChanged(m_stageManagerEnabled);

        if (m_dbus && m_dbus->isValid()) {
            m_dbus->call(QStringLiteral("set_stage_manager_enabled"), enabled);
        }
    }
}

void SettingsConfigController::setAppearanceMode(const QString &mode)
{
    if (m_appearanceMode != mode) {
        m_appearanceMode = mode;
        Q_EMIT appearanceModeChanged(m_appearanceMode);

        if (m_dbus && m_dbus->isValid()) {
            m_dbus->call(QStringLiteral("set_appearance_mode"), mode);
        }
    }
}

void SettingsConfigController::setThemeName(const QString &name)
{
    if (m_themeName != name) {
        m_themeName = name;
        Q_EMIT themeNameChanged(m_themeName);

        if (m_dbus && m_dbus->isValid()) {
            m_dbus->call(QStringLiteral("set_theme"), name);
        }
    }
}

void SettingsConfigController::setIconTheme(const QString &theme)
{
    if (m_iconTheme != theme) {
        m_iconTheme = theme;
        Q_EMIT iconThemeChanged(m_iconTheme);

        if (m_dbus && m_dbus->isValid()) {
            m_dbus->call(QStringLiteral("set_icon_theme"), theme);
        }
    }
}

void SettingsConfigController::setWallpaper(const QString &wp)
{
    if (m_wallpaper != wp) {
        m_wallpaper = wp;
        Q_EMIT wallpaperChanged(m_wallpaper);
    }
}

void SettingsConfigController::setDockSize(int size)
{
    if (m_dockSize != size) {
        m_dockSize = size;
        Q_EMIT dockSizeChanged(m_dockSize);
    }
}

void SettingsConfigController::setMagnificationEnabled(bool enabled)
{
    if (m_magnificationEnabled != enabled) {
        m_magnificationEnabled = enabled;
        Q_EMIT magnificationEnabledChanged(m_magnificationEnabled);
    }
}

} // namespace Quartz::Settings
