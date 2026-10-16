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
            text: qsTr("Apariencia")
            font.bold: true
            font.pixelSize: 22
            color: "#1C1C1E"
        }

        // Selector de Modo: Claro, Oscuro, Auto
        Rectangle {
            Layout.fillWidth: true
            height: appCol.implicitHeight + 24
            radius: 10
            color: "#FFFFFF"
            border.color: "#E5E5EA"

            ColumnLayout {
                id: appCol
                anchors.fill: parent
                anchors.margins: 16
                spacing: 16

                Text {
                    text: qsTr("Aspecto")
                    font.bold: true
                    font.pixelSize: 13
                    color: "#8E8E93"
                }

                RowLayout {
                    spacing: 20
                    Layout.alignment: Qt.AlignHCenter

                    // Opción Claro
                    ColumnLayout {
                        spacing: 6
                        Rectangle {
                            width: 80; height: 50; radius: 6
                            color: "#EBEBF0"
                            border.color: (typeof settingsConfigCtrl !== "undefined" && settingsConfigCtrl.appearanceMode === "Light") ? "#007AFF" : "#D1D1D6"
                            border.width: (typeof settingsConfigCtrl !== "undefined" && settingsConfigCtrl.appearanceMode === "Light") ? 2 : 1
                            MouseArea {
                                anchors.fill: parent
                                onClicked: if (typeof settingsConfigCtrl !== "undefined") settingsConfigCtrl.appearanceMode = "Light"
                            }
                            Text { anchors.centerIn: parent; text: "☀️"; font.pixelSize: 20 }
                        }
                        Text { Layout.alignment: Qt.AlignHCenter; text: qsTr("Claro"); font.pixelSize: 12; color: "#1C1C1E" }
                    }

                    // Opción Oscuro
                    ColumnLayout {
                        spacing: 6
                        Rectangle {
                            width: 80; height: 50; radius: 6
                            color: "#1C1C1E"
                            border.color: (typeof settingsConfigCtrl !== "undefined" && settingsConfigCtrl.appearanceMode === "Dark") ? "#007AFF" : "#3A3A3C"
                            border.width: (typeof settingsConfigCtrl !== "undefined" && settingsConfigCtrl.appearanceMode === "Dark") ? 2 : 1
                            MouseArea {
                                anchors.fill: parent
                                onClicked: if (typeof settingsConfigCtrl !== "undefined") settingsConfigCtrl.appearanceMode = "Dark"
                            }
                            Text { anchors.centerIn: parent; text: "🌙"; font.pixelSize: 20 }
                        }
                        Text { Layout.alignment: Qt.AlignHCenter; text: qsTr("Oscuro"); font.pixelSize: 12; color: "#1C1C1E" }
                    }

                    // Opción Auto
                    ColumnLayout {
                        spacing: 6
                        Rectangle {
                            width: 80; height: 50; radius: 6
                            color: "#A2A2A7"
                            border.color: (typeof settingsConfigCtrl !== "undefined" && settingsConfigCtrl.appearanceMode === "Auto") ? "#007AFF" : "#D1D1D6"
                            border.width: (typeof settingsConfigCtrl !== "undefined" && settingsConfigCtrl.appearanceMode === "Auto") ? 2 : 1
                            MouseArea {
                                anchors.fill: parent
                                onClicked: if (typeof settingsConfigCtrl !== "undefined") settingsConfigCtrl.appearanceMode = "Auto"
                            }
                            Text { anchors.centerIn: parent; text: "🌗"; font.pixelSize: 20 }
                        }
                        Text { Layout.alignment: Qt.AlignHCenter; text: qsTr("Automático"); font.pixelSize: 12; color: "#1C1C1E" }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                // Color de acento
                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Color de acento"); font.pixelSize: 14; color: "#1C1C1E" }

                    RowLayout {
                        spacing: 8
                        Repeater {
                            model: ["#007AFF", "#AF52DE", "#FF2D55", "#FF9500", "#FFCC00", "#34C759", "#8E8E93"]
                            delegate: Rectangle {
                                width: 22; height: 22; radius: 11
                                color: modelData
                                border.color: "#FFFFFF"
                                border.width: 2
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                }
                            }
                        }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#E5E5EA" }

                // Tema de ventanas y widgets
                RowLayout {
                    Layout.fillWidth: true
                    Text { Layout.fillWidth: true; text: qsTr("Tema del Sistema"); font.pixelSize: 14; color: "#1C1C1E" }
                    ComboBox {
                        model: ["Aqua", "Orchis", "Orchis-Dark"]
                        currentIndex: typeof settingsConfigCtrl !== "undefined" ? Math.max(0, model.indexOf(settingsConfigCtrl.themeName)) : 0
                        onActivated: {
                            if (typeof settingsConfigCtrl !== "undefined") {
                                settingsConfigCtrl.themeName = currentText
                            }
                        }
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
