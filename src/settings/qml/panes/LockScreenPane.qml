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
            text: qsTr("Pantalla de bloqueo")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        Rectangle {
            Layout.fillWidth: true
            height: lockCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: lockCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Iniciar salvapantallas tras inactividad"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: [qsTr("Nunca"), qsTr("2 minutos"), qsTr("5 minutos"), qsTr("10 minutos"), qsTr("15 minutos")]
                        currentIndex: 2
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Apagar pantalla con batería tras inactividad"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: [qsTr("1 minuto"), qsTr("2 minutos"), qsTr("5 minutos"), qsTr("15 minutos")]
                        currentIndex: 1
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Solicitar contraseña tras iniciarse el salvapantallas"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: [qsTr("Inmediatamente"), qsTr("Tras 5 segundos"), qsTr("Tras 1 minuto"), qsTr("Tras 15 minutos")]
                        currentIndex: 0
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
