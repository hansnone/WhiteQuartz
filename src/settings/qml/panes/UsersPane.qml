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
            text: qsTr("Usuarios y grupos")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        Rectangle {
            Layout.fillWidth: true
            height: usersCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: usersCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Rectangle {
                        width: 40; height: 40; radius: 20; color: "#007AFF"
                        Text { anchors.centerIn: parent; text: "P"; color: "white"; font.bold: true; font.pixelSize: 18 }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text { text: qsTr("Pedro (Usuario actual)"); font.bold: true; font.pixelSize: 14; color: "#1C1C1E" }
                        Text { text: qsTr("Administrador"); font.pixelSize: 12; color: "#8E8E93" }
                    }

                    Button { text: qsTr("Detalles...") }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Inicio de sesión automático"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: [qsTr("Desactivado"), qsTr("Pedro")]
                    }
                }
            }
        }

        Button {
            text: qsTr("Añadir cuenta de usuario...")
        }

        Item { Layout.fillHeight: true }
    }
}
