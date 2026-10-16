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
            text: qsTr("General")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        // Información del sistema / Almacenamiento
        Rectangle {
            Layout.fillWidth: true
            height: generalCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: generalCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 14

                RowLayout {
                    Layout.fillWidth: true
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text { text: qsTr("QuartzDE Sonoma"); font.bold: true; font.pixelSize: 15; color: "#1C1C1E" }
                        Text { text: qsTr("Versión 0.1.0 (Arquitectura x86_64)"); font.pixelSize: 12; color: "#8E8E93" }
                    }
                    Button { text: qsTr("Actualización de software...") }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                // Barra de almacenamiento estilo macOS
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: qsTr("Almacenamiento"); font.bold: true; font.pixelSize: 13; color: "#1C1C1E" }
                        Item { Layout.fillWidth: true }
                        Text { text: qsTr("142 GB disponibles de 512 GB"); font.pixelSize: 12; color: "#8E8E93" }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 14
                        radius: 7
                        color: "#E5E5EA"

                        Row {
                            anchors.fill: parent
                            Rectangle { width: parent.width * 0.35; height: parent.height; radius: 7; color: "#007AFF" } // Apps
                            Rectangle { width: parent.width * 0.20; height: parent.height; color: "#FF9500" } // Docs
                            Rectangle { width: parent.width * 0.15; height: parent.height; color: "#AF52DE" } // Sistema
                        }
                    }

                    RowLayout {
                        spacing: 16
                        RowLayout { spacing: 4; Rectangle { width: 8; height: 8; radius: 4; color: "#007AFF" }; Text { text: qsTr("Apps"); font.pixelSize: 11; color: "#8E8E93" } }
                        RowLayout { spacing: 4; Rectangle { width: 8; height: 8; radius: 4; color: "#FF9500" }; Text { text: qsTr("Documentos"); font.pixelSize: 11; color: "#8E8E93" } }
                        RowLayout { spacing: 4; Rectangle { width: 8; height: 8; radius: 4; color: "#AF52DE" }; Text { text: qsTr("Sistema"); font.pixelSize: 11; color: "#8E8E93" } }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Fecha y hora automáticas"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch { checked: true }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Idioma y región"); font.pixelSize: 14; color: "#1C1C1E" }
                    Text { text: qsTr("Español (España)"); font.pixelSize: 13; color: "#8E8E93" }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
