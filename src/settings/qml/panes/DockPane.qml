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
            text: qsTr("Escritorio y Dock")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        Rectangle {
            Layout.fillWidth: true
            height: dockCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: dockCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 14

                // Tamaño del Dock
                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Tamaño del Dock"); font.pixelSize: 14; color: "#1C1C1E" }
                    Slider {
                        id: dockSizeSlider
                        Layout.preferredWidth: 200
                        from: 32; to: 96
                        value: typeof settingsConfigCtrl !== "undefined" ? settingsConfigCtrl.dockSize : 48
                        onMoved: if (typeof settingsConfigCtrl !== "undefined") settingsConfigCtrl.dockSize = Math.round(value)
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                // Ampliación
                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Ampliación en hover"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch {
                        id: magSwitch
                        checked: typeof settingsConfigCtrl !== "undefined" ? settingsConfigCtrl.magnificationEnabled : true
                        onToggled: if (typeof settingsConfigCtrl !== "undefined") settingsConfigCtrl.magnificationEnabled = checked
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                // Stage Manager
                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Activar Stage Manager"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch {
                        id: smSwitch
                        checked: typeof settingsConfigCtrl !== "undefined" ? settingsConfigCtrl.stageManagerEnabled : false
                        onToggled: if (typeof settingsConfigCtrl !== "undefined") settingsConfigCtrl.stageManagerEnabled = checked
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                // Ocultar automáticamente
                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Ocultar y mostrar el Dock automáticamente"); font.pixelSize: 14; color: "#1C1C1E" }
                    Switch { checked: false }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                // Efecto de minimización
                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Efecto de minimización"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: [qsTr("Efecto aladino (Genie)"), qsTr("Efecto escala")]
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
