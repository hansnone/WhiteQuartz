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
            text: qsTr("ID de Usuario")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        // Tarjeta de perfil
        Rectangle {
            Layout.fillWidth: true
            height: 90
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 16

                Rectangle {
                    width: 56
                    height: 56
                    radius: 28
                    color: "#007AFF"
                    Text {
                        anchors.centerIn: parent
                        text: "P"
                        color: "white"
                        font.pixelSize: 24
                        font.bold: true
                    }
                }

                ColumnLayout {
                    spacing: 4
                    Text {
                        text: qsTr("Pedro")
                        font.bold: true
                        font.pixelSize: 16
                        color: "#1C1C1E"
                    }
                    Text {
                        text: qsTr("pedro@quartz.local")
                        font.pixelSize: 13
                        color: "#8E8E93"
                    }
                }
            }
        }

        // Ajustes de cuenta
        Rectangle {
            Layout.fillWidth: true
            height: accountCol.implicitHeight + 32
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: accountCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        Layout.fillWidth: true
                        text: qsTr("Inicio de sesión y seguridad")
                        font.pixelSize: 14
                        color: "#1C1C1E"
                    }
                    Button {
                        text: qsTr("Cambiar contraseña...")
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        Layout.fillWidth: true
                        text: qsTr("Sincronización local Quartz")
                        font.pixelSize: 14
                        color: "#1C1C1E"
                    }
                    Switch { checked: true }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
