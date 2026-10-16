import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: bannerWindow
    visible: notificationModel ? notificationModel.activeBanners.length > 0 : false
    width: 360
    height: Math.min(600, bannerCol.implicitHeight + 20)
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool

    // Posición superior derecha
    x: Screen.width - width - 20
    y: 34

    Column {
        id: bannerCol
        width: parent.width
        spacing: 10

        Repeater {
            model: notificationModel ? notificationModel.activeBanners : []

            delegate: Rectangle {
                id: card
                width: 360
                height: contentLayout.implicitHeight + 24
                radius: 16
                color: "#E62C2C2E" // Frosted glass oscuro macOS
                border.color: "#33FFFFFF"
                border.width: 1

                ColumnLayout {
                    id: contentLayout
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 6

                    // Cabecera: Icono, Nombre de app, tiempo, botón cerrar
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Rectangle {
                            width: 20
                            height: 20
                            radius: 4
                            color: "#007AFF"
                            Text {
                                anchors.centerIn: parent
                                text: modelData.appName ? modelData.appName.charAt(0).toUpperCase() : "Q"
                                color: "white"
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }

                        Text {
                            text: modelData.appName
                            color: "#EBEBF5"
                            font.pixelSize: 12
                            font.bold: true
                        }

                        Item { Layout.fillWidth: true }

                        Text {
                            text: modelData.time || qsTr("ahora")
                            color: "#8E8E93"
                            font.pixelSize: 11
                        }

                        // Botón cerrar
                        Rectangle {
                            width: 18
                            height: 18
                            radius: 9
                            color: closeMouse.containsMouse ? "#55FFFFFF" : "#22FFFFFF"

                            Text {
                                anchors.centerIn: parent
                                text: "×"
                                color: "white"
                                font.pixelSize: 13
                            }

                            MouseArea {
                                id: closeMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: notificationModel.dismiss(modelData.id)
                            }
                        }
                    }

                    // Título / Resumen
                    Text {
                        Layout.fillWidth: true
                        text: modelData.summary
                        color: "#FFFFFF"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                        elide: Text.ElideRight
                        maximumLineCount: 2
                    }

                    // Mensaje / Cuerpo
                    Text {
                        Layout.fillWidth: true
                        text: modelData.body
                        color: "#D1D1D6"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                        elide: Text.ElideRight
                        maximumLineCount: 3
                    }

                    // Acciones si existen
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        visible: modelData.actions && modelData.actions.length > 1

                        Repeater {
                            model: {
                                var res = [];
                                if (modelData.actions) {
                                    for (var i = 0; i < modelData.actions.length; i += 2) {
                                        res.push({ key: modelData.actions[i], label: modelData.actions[i+1] });
                                    }
                                }
                                return res;
                            }

                            delegate: Button {
                                text: modelData.label
                                onClicked: notificationModel.invokeAction(card.modelData.id, modelData.key)
                                background: Rectangle {
                                    radius: 6
                                    color: "#3A3A3C"
                                }
                                contentItem: Text {
                                    text: parent.text
                                    color: "white"
                                    font.pixelSize: 11
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
