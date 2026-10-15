import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    id: menuBarWin
    title: "QuartzDE MenuBar"
    height: 24
    width: Screen.desktopAvailableWidth > 0 ? Screen.desktopAvailableWidth : 1280
    visible: true
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
                MenuItem { text: qsTr("Acerca de este equipo"); onTriggered: menuBarCtrl.triggerAppleMenuAction("about") }
                MenuItem { text: qsTr("Ajustes del Sistema..."); onTriggered: menuBarCtrl.triggerAppleMenuAction("settings") }
                MenuSeparator {}
                MenuItem { text: qsTr("Reposo"); onTriggered: menuBarCtrl.triggerAppleMenuAction("sleep") }
                MenuItem { text: qsTr("Reiniciar..."); onTriggered: menuBarCtrl.triggerAppleMenuAction("reboot") }
                MenuItem { text: qsTr("Apagar equipo..."); onTriggered: menuBarCtrl.triggerAppleMenuAction("shutdown") }
            }
        }

        // Título de la aplicación activa en negrita (SF Pro Bold)
        Text {
            text: menuBarCtrl.activeAppTitle
            font.bold: true
            font.pixelSize: 13
            color: "#1C1C1E"
        }

        // Menús comunes de aplicación (introspección con tecla Alt)
        Text { text: qsTr("Archivo"); font.pixelSize: 13; color: "#1C1C1E" }
        Text { text: qsTr("Edición"); font.pixelSize: 13; color: "#1C1C1E" }
        Text { text: qsTr("Visualización"); font.pixelSize: 13; color: "#1C1C1E" }
        Text { text: qsTr("Ventana"); font.pixelSize: 13; color: "#1C1C1E" }
        Text { text: qsTr("Ayuda"); font.pixelSize: 13; color: "#1C1C1E" }

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

        // Iconos de estado a la derecha y Reloj (invoca Centro de Notificaciones)
        Text {
            text: "􀊫  􀙇  􀕒"
            font.pixelSize: 13
            color: "#1C1C1E"
        }

        Text {
            text: menuBarCtrl.formattedTime
            font.pixelSize: 13
            color: "#1C1C1E"
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: menuBarCtrl.toggleNotificationCenter()
            }
        }
    }
}
