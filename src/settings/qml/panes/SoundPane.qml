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
            text: qsTr("Sonido")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        Rectangle {
            Layout.fillWidth: true
            height: soundCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: soundCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Volumen de salida"); font.pixelSize: 14; color: "#1C1C1E" }
                    Slider {
                        Layout.preferredWidth: 200
                        from: 0; to: 100; value: 75
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Dispositivo de salida"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: [qsTr("Altavoces integrados"), qsTr("Auriculares (Jack 3.5mm)"), qsTr("Salida HDMI/DisplayPort")]
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Sonido de alerta"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: ["Basso", "Blow", "Bottle", "Frog", "Funk", "Glass", "Hero", "Morse", "Ping", "Pop", "Purr", "Sosumi", "Submarine", "Tink"]
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
