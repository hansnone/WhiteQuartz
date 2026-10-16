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
            text: qsTr("Red")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        Rectangle {
            Layout.fillWidth: true
            height: netCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: netCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "🟢"; font.pixelSize: 12 }
                    Text { Layout.fillWidth: true; text: qsTr("Ethernet"); font.bold: true; font.pixelSize: 13; color: "#1C1C1E" }
                    Text { text: qsTr("Conectado (1000 Mbps)"); font.pixelSize: 12; color: "#8E8E93" }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "🛡️"; font.pixelSize: 14 }
                    Text { Layout.fillWidth: true; text: qsTr("Cortafuegos del sistema"); font.pixelSize: 13; color: "#1C1C1E" }
                    Switch { checked: true }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "🌐"; font.pixelSize: 14 }
                    Text { Layout.fillWidth: true; text: qsTr("VPN y Proxies"); font.pixelSize: 13; color: "#1C1C1E" }
                    Button { text: qsTr("Configurar...") }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
