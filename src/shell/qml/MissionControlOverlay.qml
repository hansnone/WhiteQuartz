import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    id: missionControlWin
    title: "Mission Control"
    width: Screen.desktopAvailableWidth > 0 ? Screen.desktopAvailableWidth : 1920
    height: Screen.desktopAvailableHeight > 0 ? Screen.desktopAvailableHeight : 1080
    visible: typeof missionControlCtrl !== "undefined" ? missionControlCtrl.active : false
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    Rectangle {
        id: bgOverlay
        anchors.fill: parent
        color: "#99000000"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (typeof missionControlCtrl !== "undefined") {
                    missionControlCtrl.active = false
                }
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 20

            // 1. Barra de Spaces (Escritorios Virtuales) superior
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: 110
                Layout.preferredWidth: Math.min(missionControlWin.width - 48, spacesRow.implicitWidth + 40)
                radius: 16
                color: "#40000000"
                border.color: "#33FFFFFF"
                border.width: 1

                RowLayout {
                    id: spacesRow
                    anchors.centerIn: parent
                    spacing: 16

                    Repeater {
                        model: typeof missionControlCtrl !== "undefined" ? missionControlCtrl.spaces : []

                        delegate: Rectangle {
                            id: spaceThumb
                            Layout.preferredWidth: 130
                            Layout.preferredHeight: 84
                            radius: 10
                            color: modelData.active ? "#33007AFF" : "#26FFFFFF"
                            border.color: modelData.active ? "#007AFF" : "#33FFFFFF"
                            border.width: modelData.active ? 2 : 1
                            scale: spaceHover.containsMouse ? 1.05 : 1.0
                            Behavior on scale { NumberAnimation { duration: 120 } }

                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 4

                                Text {
                                    Layout.alignment: Qt.AlignHCenter
                                    text: "🖥️"
                                    font.pixelSize: 22
                                }

                                Text {
                                    Layout.alignment: Qt.AlignHCenter
                                    text: modelData.name
                                    font.pixelSize: 11
                                    font.weight: modelData.active ? Font.Bold : Font.Normal
                                    color: "#FFFFFF"
                                }
                            }

                            // Botón de cerrar espacio si hay más de 1
                            Rectangle {
                                anchors.top: parent.top
                                anchors.right: parent.right
                                anchors.margins: 4
                                width: 16
                                height: 16
                                radius: 8
                                color: "#80FF3B30"
                                visible: spaceHover.containsMouse && (typeof missionControlCtrl !== "undefined" && missionControlCtrl.spaces.length > 1)

                                Text {
                                    anchors.centerIn: parent
                                    text: "✕"
                                    font.pixelSize: 9
                                    color: "#FFFFFF"
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        if (typeof missionControlCtrl !== "undefined") {
                                            missionControlCtrl.removeSpace(index)
                                        }
                                    }
                                }
                            }

                            MouseArea {
                                id: spaceHover
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    if (typeof missionControlCtrl !== "undefined") {
                                        missionControlCtrl.switchToSpace(index)
                                    }
                                }
                            }
                        }
                    }

                    // Botón para agregar un nuevo Space (+)
                    Rectangle {
                        Layout.preferredWidth: 44
                        Layout.preferredHeight: 84
                        radius: 10
                        color: addHover.containsMouse ? "#40FFFFFF" : "#20FFFFFF"
                        border.color: "#33FFFFFF"
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: "+"
                            font.pixelSize: 24
                            font.weight: Font.Light
                            color: "#FFFFFF"
                        }

                        MouseArea {
                            id: addHover
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (typeof missionControlCtrl !== "undefined") {
                                    missionControlCtrl.addSpace()
                                }
                            }
                        }
                    }
                }
            }

            // 2. Área principal de ventanas (Rejilla de Mission Control)
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Si no hay ventanas activas registradas en la vista
                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 12
                    visible: typeof missionControlCtrl === "undefined" || missionControlCtrl.windows.length === 0

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "🪟"
                        font.pixelSize: 48
                        opacity: 0.6
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Mission Control activo")
                        font.pixelSize: 18
                        font.bold: true
                        color: "#E0FFFFFF"
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Pulsa Super+Up o haz clic en cualquier lugar para volver")
                        font.pixelSize: 13
                        color: "#99FFFFFF"
                    }
                }
            }
        }
    }
}
