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
            text: qsTr("Pantallas")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        // Vista de pantalla integrada
        Rectangle {
            Layout.fillWidth: true
            height: 120
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            RowLayout {
                anchors.centerIn: parent
                spacing: 16

                Rectangle {
                    width: 100; height: 65; radius: 4; color: "#2C2C2E"
                    border.color: "#8E8E93"; border.width: 2
                    Text { anchors.centerIn: parent; text: "1920 × 1080\n60 Hz"; font.pixelSize: 10; color: "white"; horizontalAlignment: Text.AlignHCenter }
                }

                ColumnLayout {
                    spacing: 4
                    Text { text: qsTr("Pantalla integrada (Display-1)"); font.bold: true; font.pixelSize: 14; color: "#1C1C1E" }
                    Text { text: qsTr("Predeterminada para la pantalla"); font.pixelSize: 12; color: "#8E8E93" }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: dispCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: dispCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Resolución"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: ["1920 × 1080 (16:9)", "2560 × 1440 (2K)", "3840 × 2160 (4K)", "1680 × 1050", "1280 × 720"]
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Tasa de refresco"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: ["60 Hz", "75 Hz", "120 Hz (ProMotion)", "144 Hz"]
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Night Shift (Luz cálida nocturna)"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch { checked: false }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
