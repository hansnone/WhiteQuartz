import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true

    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        Text {
            text: qsTr("Fondo de pantalla")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        // Vista previa grande del fondo actual
        Rectangle {
            Layout.fillWidth: true
            height: 180
            radius: 12
            color: "#2C2C2E"
            clip: true

            Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                source: (typeof settingsConfigCtrl !== "undefined" && settingsConfigCtrl.wallpaper) ? settingsConfigCtrl.wallpaper : "file:///usr/share/backgrounds/quartz.png"
            }

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 36
                color: "#99000000"

                Text {
                    anchors.centerIn: parent
                    text: qsTr("Fondo de pantalla actual de QuartzDE")
                    color: "white"
                    font.pixelSize: 12
                }
            }
        }

        Text {
            text: qsTr("Fondos de pantalla de Sonoma y Sequoia")
            font.bold: true
            font.pixelSize: 14
            color: "#8E8E93"
        }

        // Grid de selección de fondos
        Rectangle {
            Layout.fillWidth: true
            height: 140
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12

                Repeater {
                    model: [
                        { name: "Sonoma", color: "#007AFF" },
                        { name: "Sequoia", color: "#34C759" },
                        { name: "Ventura", color: "#FF9500" },
                        { name: "Monterey", color: "#AF52DE" },
                        { name: "Quartz Dark", color: "#1C1C1E" }
                    ]

                    delegate: Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: 8
                        color: modelData.color

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (typeof settingsConfigCtrl !== "undefined") {
                                    settingsConfigCtrl.wallpaper = "file:///usr/share/backgrounds/" + modelData.name.toLowerCase() + ".png"
                                }
                            }
                        }

                        Text {
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottomMargin: 8
                            text: modelData.name
                            color: "white"
                            font.bold: true
                            font.pixelSize: 11
                        }
                    }
                }
            }
        }

        Button {
            text: qsTr("Añadir fondo de pantalla desde archivo...")
        }

        Item { Layout.fillHeight: true }
    }
}
