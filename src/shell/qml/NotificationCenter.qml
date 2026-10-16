import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: centerWindow
    visible: notificationModel ? notificationModel.centerVisible : false
    width: 380
    height: Screen.height - 30
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool

    x: Screen.width - width - 10
    y: 28

    Rectangle {
        anchors.fill: parent
        radius: 18
        color: "#CC1C1C1E" // Frosted acrylic oscuro
        border.color: "#33FFFFFF"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 14

            // Cabecera del Centro de Notificaciones
            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: qsTr("Centro de Notificaciones")
                    color: "white"
                    font.bold: true
                    font.pixelSize: 17
                }

                Item { Layout.fillWidth: true }

                Rectangle {
                    visible: notificationModel && notificationModel.history.length > 0
                    width: clearText.implicitWidth + 16
                    height: 24
                    radius: 12
                    color: clearMouse.containsMouse ? "#44FFFFFF" : "#22FFFFFF"

                    Text {
                        id: clearText
                        anchors.centerIn: parent
                        text: qsTr("Borrar todo")
                        color: "#EBEBF5"
                        font.pixelSize: 11
                    }

                    MouseArea {
                        id: clearMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: notificationModel.clearAll()
                    }
                }
            }

            // Lista de Notificaciones
            ListView {
                id: historyList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                spacing: 8
                model: notificationModel ? notificationModel.history : []

                delegate: Rectangle {
                    width: historyList.width
                    height: itemCol.implicitHeight + 20
                    radius: 12
                    color: "#2C2C2E"

                    ColumnLayout {
                        id: itemCol
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 4

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            Rectangle {
                                width: 16
                                height: 16
                                radius: 4
                                color: "#007AFF"
                                Text {
                                    anchors.centerIn: parent
                                    text: modelData.appName ? modelData.appName.charAt(0).toUpperCase() : "Q"
                                    color: "white"
                                    font.pixelSize: 9
                                    font.bold: true
                                }
                            }

                            Text {
                                text: modelData.appName
                                color: "#8E8E93"
                                font.pixelSize: 11
                                font.bold: true
                            }

                            Item { Layout.fillWidth: true }

                            Text {
                                text: modelData.time
                                color: "#8E8E93"
                                font.pixelSize: 10
                            }
                        }

                        Text {
                            text: modelData.summary
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                        }

                        Text {
                            text: modelData.body
                            color: "#C7C7CC"
                            font.pixelSize: 11
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                        }
                    }
                }

                // Placeholder cuando no hay notificaciones
                Text {
                    anchors.centerIn: parent
                    visible: historyList.count === 0
                    text: qsTr("No hay notificaciones recientes")
                    color: "#8E8E93"
                    font.pixelSize: 13
                }
            }

            // Widget Inferior: Calendario y Reloj
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 90
                radius: 14
                color: "#2C2C2E"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 16

                    ColumnLayout {
                        spacing: 2
                        Text {
                            text: Qt.formatDate(new Date(), "dddd").toUpperCase()
                            color: "#FF453A"
                            font.pixelSize: 11
                            font.bold: true
                        }
                        Text {
                            text: Qt.formatDate(new Date(), "d")
                            color: "white"
                            font.pixelSize: 32
                            font.bold: true
                        }
                    }

                    Rectangle {
                        width: 1
                        Layout.fillHeight: true
                        color: "#3A3A3C"
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        Text {
                            text: Qt.formatTime(new Date(), "hh:mm")
                            color: "white"
                            font.pixelSize: 22
                            font.bold: true
                        }
                        Text {
                            text: qsTr("Sin eventos hoy")
                            color: "#8E8E93"
                            font.pixelSize: 12
                        }
                    }
                }
            }
        }
    }
}
