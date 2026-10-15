import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: shellRoot
    width: (typeof Screen !== "undefined" && Screen.desktopAvailableWidth > 0) ? Screen.desktopAvailableWidth : 1280
    height: (typeof Screen !== "undefined" && Screen.desktopAvailableHeight > 0) ? Screen.desktopAvailableHeight : 800
    visible: true
    title: "QuartzDE Shell"
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnBottomHint

    // 1. Barra de menús superior (24px fija - Ley de Fitts)
    Rectangle {
        id: menuBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 24
        color: "#E6F6F6F6"

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

    // 2. Dock inferior flotante con magnificación gaussiana y rebote
    Rectangle {
        id: dockContainer
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.horizontalCenter: parent.horizontalCenter
        height: 64
        width: dockRow.implicitWidth + 24
        radius: 18
        color: "#CCFFFFFF"
        border.color: "#33FFFFFF"
        border.width: 1

        property real mouseXPos: -1000
        property bool isHovered: false

        HoverHandler {
            id: dockHover
            onHoveredChanged: {
                dockContainer.isHovered = dockHover.hovered
                if (!dockHover.hovered) {
                    dockContainer.mouseXPos = -1000
                }
            }
            onPointChanged: {
                if (dockHover.hovered) {
                    dockContainer.mouseXPos = dockHover.point.position.x
                }
            }
        }

        Row {
            id: dockRow
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8

            Repeater {
                model: dockCtrl.defaultApps()
                delegate: Item {
                    id: dockItem
                    property real baseSize: dockCtrl.baseIconSize
                    property real targetIconSize: {
                        if (modelData.isSeparator) return 8
                        if (!dockContainer.isHovered || dockContainer.mouseXPos < 0) {
                            return baseSize
                        }
                        var itemCenterX = dockRow.x + dockItem.x + baseSize / 2
                        var dist = Math.abs(dockContainer.mouseXPos - itemCenterX)
                        return dockCtrl.calculateGaussianMagnification(dist, 55.0)
                    }

                    width: modelData.isSeparator ? 8 : (targetIconSize + 2)
                    height: 52

                    Behavior on width {
                        NumberAnimation { duration: 60; easing.type: Easing.OutQuad }
                    }

                    // Separador vertical fino
                    Rectangle {
                        visible: modelData.isSeparator
                        anchors.centerIn: parent
                        width: 1
                        height: 38
                        color: "#B0B0B5"
                    }

                    // Icono de app
                    Rectangle {
                        id: appIcon
                        visible: !modelData.isSeparator
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 4
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: targetIconSize
                        height: targetIconSize
                        radius: Math.min(18, Math.round(width * 0.23))
                        color: "#007AFF"

                        Behavior on width {
                            NumberAnimation { duration: 60; easing.type: Easing.OutQuad }
                        }
                        Behavior on height {
                            NumberAnimation { duration: 60; easing.type: Easing.OutQuad }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: modelData.name ? modelData.name.charAt(0) : ""
                            color: "white"
                            font.bold: true
                            font.pixelSize: Math.max(14, Math.round(appIcon.width * 0.38))
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (modelData.cmd === "quartz-spotlight") {
                                    if (typeof spotlightCtrl !== "undefined") {
                                        spotlightCtrl.toggle()
                                        return
                                    }
                                }
                                dockCtrl.launchApp(modelData.cmd)
                            }
                        }

                        // Animación de rebote parabólico
                        SequentialAnimation {
                            id: bounceAnim
                            loops: 2
                            NumberAnimation {
                                target: appIcon
                                property: "anchors.bottomMargin"
                                to: 4 + 20
                                duration: 180
                                easing.type: Easing.OutQuad
                            }
                            NumberAnimation {
                                target: appIcon
                                property: "anchors.bottomMargin"
                                to: 4
                                duration: 180
                                easing.type: Easing.InQuad
                            }
                        }

                        Connections {
                            target: dockCtrl
                            function onAppBouncingRequested(cmd) {
                                if (modelData.cmd === cmd) {
                                    bounceAnim.restart()
                                }
                            }
                        }
                    }

                    // Indicador de app activa: punto luminoso de 4pt
                    Rectangle {
                        visible: !modelData.isSeparator && modelData.running
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 4
                        height: 4
                        radius: 2
                        color: "#1C1C1E"
                    }
                }
            }
        }
    }
}
