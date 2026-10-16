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
            text: qsTr("Teclado")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        Rectangle {
            Layout.fillWidth: true
            height: kbCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: kbCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Velocidad de repetición de teclas"); font.pixelSize: 14; color: "#1C1C1E" }
                    Slider { Layout.preferredWidth: 200; from: 1; to: 10; value: 8 }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Espera hasta la repetición"); font.pixelSize: 14; color: "#1C1C1E" }
                    Slider { Layout.preferredWidth: 200; from: 1; to: 10; value: 6 }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Distribución y fuentes de entrada"); font.pixelSize: 14; color: "#1C1C1E" }
                    Text { text: qsTr("Español (ISO)"); font.pixelSize: 13; color: "#8E8E93" }
                }
            }
        }

        Button {
            text: qsTr("Atajos de teclado de sesión (Modo PC/Windows)...")
        }

        Item { Layout.fillHeight: true }
    }
}
