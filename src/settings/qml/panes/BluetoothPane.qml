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
            text: qsTr("Bluetooth")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        Rectangle {
            Layout.fillWidth: true
            height: 60
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 16

                Text {
                    Layout.fillWidth: true
                    text: qsTr("Bluetooth")
                    font.pixelSize: 14
                    font.bold: true
                    color: "#1C1C1E"
                }

                Switch { checked: true }
            }
        }

        Text {
            text: qsTr("Mis dispositivos")
            font.bold: true
            font.pixelSize: 14
            color: "#8E8E93"
        }

        Rectangle {
            Layout.fillWidth: true
            height: btCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: btCol
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "🎧"; font.pixelSize: 14 }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text { text: qsTr("AirPods Pro"); font.bold: true; font.pixelSize: 13; color: "#1C1C1E" }
                        Text { text: qsTr("Conectado"); font.pixelSize: 11; color: "#007AFF" }
                    }
                    Button { text: qsTr("Desconectar") }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "⌨️"; font.pixelSize: 14 }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text { text: qsTr("Magic Keyboard"); font.bold: true; font.pixelSize: 13; color: "#1C1C1E" }
                        Text { text: qsTr("Conectado (Batería 88%)"); font.pixelSize: 11; color: "#30D158" }
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
