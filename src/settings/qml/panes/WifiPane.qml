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
            text: qsTr("Wi-Fi")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        // Interruptor principal Wi-Fi
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
                    text: qsTr("Wi-Fi")
                    font.pixelSize: 14
                    font.bold: true
                    color: "#1C1C1E"
                }

                Switch {
                    id: wifiToggle
                    checked: true
                }
            }
        }

        Text {
            text: qsTr("Redes conocidas")
            font.bold: true
            font.pixelSize: 14
            color: "#8E8E93"
        }

        // Lista de redes
        Rectangle {
            Layout.fillWidth: true
            height: networksCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: networksCol
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "📶"; font.pixelSize: 14 }
                    Text {
                        Layout.fillWidth: true
                        text: qsTr("Quartz-Net (Conectada)")
                        font.bold: true
                        font.pixelSize: 13
                        color: "#007AFF"
                    }
                    Text { text: "🔒"; font.pixelSize: 12; color: "#8E8E93" }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "📶"; font.pixelSize: 14 }
                    Text {
                        Layout.fillWidth: true
                        text: qsTr("Red_Invitados_5G")
                        font.pixelSize: 13
                        color: "#1C1C1E"
                    }
                    Text { text: "🔒"; font.pixelSize: 12; color: "#8E8E93" }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
