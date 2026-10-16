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
            text: qsTr("Centro de Control")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        Rectangle {
            Layout.fillWidth: true
            height: ccCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: ccCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Mostrar Wi-Fi en la barra de menús"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch { checked: true }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Mostrar Bluetooth en la barra de menús"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch { checked: true }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Mostrar Sonido en la barra de menús"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch { checked: true }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Mostrar porcentaje de batería"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch { checked: true }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
