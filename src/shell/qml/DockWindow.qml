import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    id: dockWin
    title: "QuartzDE Dock"
    height: 88
    width: dockContainer.width + 32
    visible: true
    color: "transparent"
    flags: Qt.FramelessWindowHint

    // Posición centrada en la base de la pantalla
    x: Screen.desktopAvailableWidth > 0 ? (Screen.desktopAvailableWidth - width) / 2 : 100
    y: Screen.desktopAvailableHeight > 0 ? (Screen.desktopAvailableHeight - height) : 700

    Rectangle {
        id: dockContainer
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
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
