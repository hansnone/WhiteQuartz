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
            text: qsTr("Notificaciones")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        Rectangle {
            Layout.fillWidth: true
            height: notifCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: notifCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Permitir notificaciones del sistema"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch { checked: true }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Estilo de aviso"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: [qsTr("Banners flotantes"), qsTr("Alertas persistentes"), qsTr("Ninguno")]
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Emitir sonido al recibir notificaciones"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch { checked: true }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
