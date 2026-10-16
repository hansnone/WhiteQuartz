import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    id: menuBarWin
    title: "QuartzDE MenuBar"
    height: 24
    width: Screen.desktopAvailableWidth > 0 ? Screen.desktopAvailableWidth : 1280
    visible: false
    x: 0
    y: 0
    color: "#E6F6F6F6"
    flags: Qt.FramelessWindowHint

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 16

        // Menú Apple estático
        Text {
            text: ""
            font.pixelSize: 15
            color: "#1C1C1E"
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: appleMenu.open()
            }

            Menu {
                id: appleMenu
                MenuItem { text: qsTr("Acerca de este Mac"); onTriggered: menuBarCtrl.triggerAppleMenuAction("about") }
                MenuItem { text: qsTr("Ajustes del Sistema..."); onTriggered: menuBarCtrl.triggerAppleMenuAction("settings") }
                MenuSeparator {}
                MenuItem { text: qsTr("Forzar salida...    Ctrl+Alt+Esc"); onTriggered: menuBarCtrl.triggerAppleMenuAction("forcequit") }
                MenuSeparator {}
                MenuItem { text: qsTr("Reposo"); onTriggered: menuBarCtrl.triggerAppleMenuAction("sleep") }
                MenuItem { text: qsTr("Reiniciar..."); onTriggered: menuBarCtrl.triggerAppleMenuAction("reboot") }
                MenuItem { text: qsTr("Apagar equipo..."); onTriggered: menuBarCtrl.triggerAppleMenuAction("shutdown") }
                MenuSeparator {}
                MenuItem { text: qsTr("Bloquear pantalla    Super+L"); onTriggered: menuBarCtrl.triggerAppleMenuAction("lock") }
                MenuItem { text: qsTr("Cerrar sesión Pedro...    Ctrl+Shift+Q"); onTriggered: menuBarCtrl.triggerAppleMenuAction("logout") }
            }
        }

        // Título de la aplicación activa en negrita (SF Pro Bold)
        Text {
            text: menuBarCtrl.activeAppTitle
            font.bold: true
            font.pixelSize: 13
            color: "#1C1C1E"
        }

        // Menús de la aplicación activa (AppMenu / DBusMenu con fallback dinámico)
        Repeater {
            model: menuBarCtrl.activeAppMenus
            delegate: Text {
                id: menuTopItem
                text: modelData.label
                font.pixelSize: 13
                color: topMouseArea.containsMouse ? "#007AFF" : "#1C1C1E"

                MouseArea {
                    id: topMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (subContextMenu.count > 0) {
                            subContextMenu.open()
                        }
                    }
                }

                Menu {
                    id: subContextMenu
                    y: parent.height + 4
                    Repeater {
                        model: modelData.children || []
                        delegate: MenuItem {
                            text: modelData.shortcut && modelData.shortcut.length > 0 ? (modelData.label + "    " + modelData.shortcut) : modelData.label
                            onTriggered: {
                                menuBarCtrl.triggerMenuAction(modelData.id)
                            }
                        }
                    }
                }
            }
        }

        Item { Layout.fillWidth: true }

        // Acceso rápido a Spotlight
        Text {
            text: "🔍"
            font.pixelSize: 13
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    if (typeof spotlightCtrl !== "undefined") {
                        spotlightCtrl.toggle()
                    }
                }
            }
        }

        // Iconos de estado a la derecha: Batería, Wi-Fi y Control Center
        RowLayout {
            spacing: 10

            Text {
                text: (typeof hardwareCtrl !== "undefined" && hardwareCtrl.wifiEnabled) ? "📶" : "❌"
                font.pixelSize: 12
                color: "#1C1C1E"
            }

            Text {
                text: (typeof hardwareCtrl !== "undefined" && hardwareCtrl.hasBattery) ? (hardwareCtrl.batteryPercentage + "%") : "100%"
                font.pixelSize: 11
                color: "#1C1C1E"
            }

            // Icono de Centro de Control (toggles/interruptores estilo macOS)
            Rectangle {
                width: 22
                height: 18
                radius: 4
                color: ccMouse.containsMouse ? "#22000000" : "transparent"

                Text {
                    anchors.centerIn: parent
                    text: "🎛️"
                    font.pixelSize: 13
                }

                MouseArea {
                    id: ccMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (typeof hardwareCtrl !== "undefined") {
                            hardwareCtrl.toggleControlCenter()
                        }
                    }
                }
            }
        }

        // Reloj (invoca Centro de Notificaciones)
        Text {
            text: menuBarCtrl.formattedTime
            font.pixelSize: 13
            color: "#1C1C1E"
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    if (typeof notificationModel !== "undefined") {
                        notificationModel.toggleCenter()
                    } else {
                        menuBarCtrl.toggleNotificationCenter()
                    }
                }
            }
        }
    }
}
