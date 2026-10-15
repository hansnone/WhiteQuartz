#include "settingsmodel.h"

namespace Quartz::Settings {

SettingsNavigationModel::SettingsNavigationModel(QObject *parent)
    : QAbstractListModel(parent)
{
    populateItems();
    m_visibleItems = m_allItems;
}

void SettingsNavigationModel::populateItems()
{
    m_allItems = {
        // Perfil Superior
        { QStringLiteral("profile"), QStringLiteral("ID de Usuario"), QStringLiteral("user-identity"), CategoryGroup::ProfileHeader, QStringLiteral("qrc:/qml/panes/ProfilePane.qml"), { QStringLiteral("cuenta"), QStringLiteral("perfil"), QStringLiteral("usuario") } },

        // Conectividad
        { QStringLiteral("wifi"), QStringLiteral("Wi-Fi"), QStringLiteral("network-wireless"), CategoryGroup::Connectivity, QStringLiteral("qrc:/qml/panes/WifiPane.qml"), { QStringLiteral("red"), QStringLiteral("inalambrico"), QStringLiteral("internet") } },
        { QStringLiteral("bluetooth"), QStringLiteral("Bluetooth"), QStringLiteral("preferences-system-bluetooth"), CategoryGroup::Connectivity, QStringLiteral("qrc:/qml/panes/BluetoothPane.qml"), { QStringLiteral("bt"), QStringLiteral("dispositivos"), QStringLiteral("auriculares") } },
        { QStringLiteral("network"), QStringLiteral("Red"), QStringLiteral("network-wired"), CategoryGroup::Connectivity, QStringLiteral("qrc:/qml/panes/NetworkPane.qml"), { QStringLiteral("ethernet"), QStringLiteral("vpn"), QStringLiteral("ip") } },

        // Notificaciones y Concentración
        { QStringLiteral("notifications"), QStringLiteral("Notificaciones"), QStringLiteral("preferences-desktop-notification"), CategoryGroup::NotificationsAndFocus, QStringLiteral("qrc:/qml/panes/NotificationsPane.qml"), { QStringLiteral("alertas"), QStringLiteral("avisos") } },
        { QStringLiteral("sound"), QStringLiteral("Sonido"), QStringLiteral("audio-volume-high"), CategoryGroup::NotificationsAndFocus, QStringLiteral("qrc:/qml/panes/SoundPane.qml"), { QStringLiteral("volumen"), QStringLiteral("altavoces"), QStringLiteral("microfono") } },
        { QStringLiteral("focus"), QStringLiteral("Concentración"), QStringLiteral("preferences-system-time"), CategoryGroup::NotificationsAndFocus, QStringLiteral("qrc:/qml/panes/FocusPane.qml"), { QStringLiteral("no molestar"), QStringLiteral("modo") } },

        // Sistema Principal
        { QStringLiteral("general"), QStringLiteral("General"), QStringLiteral("preferences-system"), CategoryGroup::SystemCore, QStringLiteral("qrc:/qml/panes/GeneralPane.qml"), { QStringLiteral("almacenamiento"), QStringLiteral("informacion"), QStringLiteral("actualizaciones") } },
        { QStringLiteral("appearance"), QStringLiteral("Apariencia"), QStringLiteral("preferences-desktop-theme"), CategoryGroup::SystemCore, QStringLiteral("qrc:/qml/panes/AppearancePane.qml"), { QStringLiteral("claro"), QStringLiteral("oscuro"), QStringLiteral("color acento"), QStringLiteral("tema") } },
        { QStringLiteral("accessibility"), QStringLiteral("Accesibilidad"), QStringLiteral("preferences-desktop-accessibility"), CategoryGroup::SystemCore, QStringLiteral("qrc:/qml/panes/AccessibilityPane.qml"), { QStringLiteral("zoom"), QStringLiteral("contraste"), QStringLiteral("subtitulos") } },
        { QStringLiteral("controlcenter"), QStringLiteral("Centro de Control"), QStringLiteral("configure"), CategoryGroup::SystemCore, QStringLiteral("qrc:/qml/panes/ControlCenterPane.qml"), { QStringLiteral("menu"), QStringLiteral("modulos"), QStringLiteral("barra superior") } },

        // Pantallas y Escritorio
        { QStringLiteral("desktop_dock"), QStringLiteral("Escritorio y Dock"), QStringLiteral("user-desktop"), CategoryGroup::DisplayAndDesktop, QStringLiteral("qrc:/qml/panes/DockPane.qml"), { QStringLiteral("dock"), QStringLiteral("magnificacion"), QStringLiteral("stacks"), QStringLiteral("stage manager") } },
        { QStringLiteral("displays"), QStringLiteral("Pantallas"), QStringLiteral("preferences-desktop-display"), CategoryGroup::DisplayAndDesktop, QStringLiteral("qrc:/qml/panes/DisplaysPane.qml"), { QStringLiteral("monitor"), QStringLiteral("resolucion"), QStringLiteral("tasa de refresco") } },
        { QStringLiteral("wallpaper"), QStringLiteral("Fondo de pantalla"), QStringLiteral("preferences-desktop-wallpaper"), CategoryGroup::DisplayAndDesktop, QStringLiteral("qrc:/qml/panes/WallpaperPane.qml"), { QStringLiteral("wallpapers"), QStringLiteral("imagen"), QStringLiteral("escritorio") } },

        // Seguridad y Entrada
        { QStringLiteral("lockscreen"), QStringLiteral("Pantalla de bloqueo"), QStringLiteral("system-lock-screen"), CategoryGroup::SecurityAndInput, QStringLiteral("qrc:/qml/panes/LockScreenPane.qml"), { QStringLiteral("bloqueo"), QStringLiteral("inactividad") } },
        { QStringLiteral("users"), QStringLiteral("Usuarios y grupos"), QStringLiteral("system-users"), CategoryGroup::SecurityAndInput, QStringLiteral("qrc:/qml/panes/UsersPane.qml"), { QStringLiteral("cuentas"), QStringLiteral("permisos"), QStringLiteral("contrasena") } },
        { QStringLiteral("keyboard"), QStringLiteral("Teclado"), QStringLiteral("preferences-desktop-keyboard"), CategoryGroup::SecurityAndInput, QStringLiteral("qrc:/qml/panes/KeyboardPane.qml"), { QStringLiteral("atajos"), QStringLiteral("idioma"), QStringLiteral("distribucion") } },
        { QStringLiteral("trackpad"), QStringLiteral("Trackpad"), QStringLiteral("input-touchpad"), CategoryGroup::SecurityAndInput, QStringLiteral("qrc:/qml/panes/TrackpadPane.qml"), { QStringLiteral("gestos"), QStringLiteral("desplazamiento"), QStringLiteral("clic") } }
    };
}

int SettingsNavigationModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_visibleItems.size();
}

QVariant SettingsNavigationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_visibleItems.size()) {
        return QVariant();
    }

    const SettingsPaneItem &item = m_visibleItems.at(index.row());
    switch (role) {
    case IdRole:
        return item.id;
    case TitleRole:
        return item.title;
    case IconRole:
        return item.iconName;
    case GroupRole:
        return static_cast<int>(item.group);
    case QmlPathRole:
        return item.qmlComponentPath;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> SettingsNavigationModel::roleNames() const
{
    return {
        { IdRole, "paneId" },
        { TitleRole, "title" },
        { IconRole, "iconName" },
        { GroupRole, "categoryGroup" },
        { QmlPathRole, "qmlPath" }
    };
}

void SettingsNavigationModel::setFilterQuery(const QString &query)
{
    beginResetModel();
    QString q = query.trimmed().toLower();
    if (q.isEmpty()) {
        m_visibleItems = m_allItems;
    } else {
        m_visibleItems.clear();
        for (const auto &item : m_allItems) {
            bool matchesTitle = item.title.toLower().contains(q);
            bool matchesKeywords = false;
            for (const auto &kw : item.keywords) {
                if (kw.toLower().contains(q)) {
                    matchesKeywords = true;
                    break;
                }
            }
            if (matchesTitle || matchesKeywords) {
                m_visibleItems.append(item);
            }
        }
    }
    endResetModel();
}

QString SettingsNavigationModel::firstMatchingPaneId() const
{
    if (!m_visibleItems.isEmpty()) {
        return m_visibleItems.first().id;
    }
    return QString();
}

} // namespace Quartz::Settings
