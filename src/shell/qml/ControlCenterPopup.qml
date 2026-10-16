import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: ccWindow
    visible: typeof hardwareCtrl !== "undefined" ? hardwareCtrl.visible : false
    width: 320
    height: ccCol.implicitHeight + 24
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool

    x: Screen.width - width - 12
    y: 28

    Rectangle {
        anchors.fill: parent
        radius: 18
        color: "#CC1C1C1E" // Frosted glass oscuro
        border.color: "#33FFFFFF"
        border.width: 1

        ColumnLayout {
            id: ccCol
            anchors.fill: parent
            anchors.margins: 12
            spacing: 10

            // Grid 2x2: Wi-Fi, Bluetooth, No Molestar, Stage Manager
            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: 8
                columnSpacing: 8

                // Wi-Fi Card
                Rectangle {
                    Layout.fillWidth: true
                    height: 60
                    radius: 12
                    color: (typeof hardwareCtrl !== "undefined" && hardwareCtrl.wifiEnabled) ? "#007AFF" : "#2C2C2E"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: if (typeof hardwareCtrl !== "undefined") hardwareCtrl.toggleWifi()
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8

                        Rectangle {
                            width: 32
                            height: 32
                            radius: 16
                            color: "#FFFFFF"
                            Text {
                                anchors.centerIn: parent
                                text: "📶"
                                font.pixelSize: 14
                            }
                        }

                        ColumnLayout {
                            spacing: 2
                            Text {
                                text: qsTr("Wi-Fi")
                                color: "white"
                                font.bold: true
                                font.pixelSize: 12
                            }
                            Text {
                                text: (typeof hardwareCtrl !== "undefined" && hardwareCtrl.wifiEnabled) ? hardwareCtrl.wifiSsid : qsTr("Desactivado")
                                color: "#EBEBF5"
                                font.pixelSize: 10
                            }
                        }
                    }
                }

                // Bluetooth Card
                Rectangle {
                    Layout.fillWidth: true
                    height: 60
                    radius: 12
                    color: (typeof hardwareCtrl !== "undefined" && hardwareCtrl.bluetoothEnabled) ? "#007AFF" : "#2C2C2E"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: if (typeof hardwareCtrl !== "undefined") hardwareCtrl.toggleBluetooth()
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8

                        Rectangle {
                            width: 32
                            height: 32
                            radius: 16
                            color: "#FFFFFF"
                            Text {
                                anchors.centerIn: parent
                                text: "ᛒ"
                                color: "#007AFF"
                                font.bold: true
                                font.pixelSize: 16
                            }
                        }

                        ColumnLayout {
                            spacing: 2
                            Text {
                                text: qsTr("Bluetooth")
                                color: "white"
                                font.bold: true
                                font.pixelSize: 12
                            }
                            Text {
                                text: (typeof hardwareCtrl !== "undefined" && hardwareCtrl.bluetoothEnabled) ? qsTr("Activado") : qsTr("Desactivado")
                                color: "#EBEBF5"
                                font.pixelSize: 10
                            }
                        }
                    }
                }

                // Concentración / No Molestar Card
                Rectangle {
                    Layout.fillWidth: true
                    height: 60
                    radius: 12
                    color: (typeof hardwareCtrl !== "undefined" && hardwareCtrl.doNotDisturb) ? "#5E5CE6" : "#2C2C2E"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: if (typeof hardwareCtrl !== "undefined") hardwareCtrl.doNotDisturb = !hardwareCtrl.doNotDisturb
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8

                        Rectangle {
                            width: 32
                            height: 32
                            radius: 16
                            color: "#FFFFFF"
                            Text {
                                anchors.centerIn: parent
                                text: "🌙"
                                font.pixelSize: 14
                            }
                        }

                        ColumnLayout {
                            spacing: 2
                            Text {
                                text: qsTr("Concentración")
                                color: "white"
                                font.bold: true
                                font.pixelSize: 12
                            }
                            Text {
                                text: (typeof hardwareCtrl !== "undefined" && hardwareCtrl.doNotDisturb) ? qsTr("No molestar") : qsTr("Desactivada")
                                color: "#EBEBF5"
                                font.pixelSize: 10
                            }
                        }
                    }
                }

                // Batería Card
                Rectangle {
                    Layout.fillWidth: true
                    height: 60
                    radius: 12
                    color: "#2C2C2E"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8

                        Rectangle {
                            width: 32
                            height: 32
                            radius: 16
                            color: "#30D158"
                            Text {
                                anchors.centerIn: parent
                                text: "⚡"
                                font.pixelSize: 14
                            }
                        }

                        ColumnLayout {
                            spacing: 2
                            Text {
                                text: qsTr("Batería")
                                color: "white"
                                font.bold: true
                                font.pixelSize: 12
                            }
                            Text {
                                text: (typeof hardwareCtrl !== "undefined") ? (hardwareCtrl.batteryPercentage + "%" + (hardwareCtrl.isCharging ? " (Cargando)" : "")) : "100%"
                                color: "#EBEBF5"
                                font.pixelSize: 10
                            }
                        }
                    }
                }
            }

            // Slider de Pantalla (Brillo)
            Rectangle {
                Layout.fillWidth: true
                height: 52
                radius: 12
                color: "#2C2C2E"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    Text {
                        text: qsTr("Pantalla")
                        color: "white"
                        font.pixelSize: 11
                        font.bold: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Text { text: "🔆"; font.pixelSize: 12; color: "white" }

                        Slider {
                            id: brightnessSlider
                            Layout.fillWidth: true
                            from: 5
                            to: 100
                            value: typeof hardwareCtrl !== "undefined" ? hardwareCtrl.brightness : 80
                            onMoved: if (typeof hardwareCtrl !== "undefined") hardwareCtrl.brightness = value
                        }
                    }
                }
            }

            // Slider de Sonido (Volumen)
            Rectangle {
                Layout.fillWidth: true
                height: 52
                radius: 12
                color: "#2C2C2E"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    Text {
                        text: qsTr("Sonido")
                        color: "white"
                        font.pixelSize: 11
                        font.bold: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Text {
                            text: (typeof hardwareCtrl !== "undefined" && hardwareCtrl.isMuted) ? "🔇" : "🔊"
                            font.pixelSize: 12
                            color: "white"

                            MouseArea {
                                anchors.fill: parent
                                onClicked: if (typeof hardwareCtrl !== "undefined") hardwareCtrl.toggleMute()
                            }
                        }

                        Slider {
                            id: volumeSlider
                            Layout.fillWidth: true
                            from: 0
                            to: 100
                            value: typeof hardwareCtrl !== "undefined" ? hardwareCtrl.volume : 75
                            onMoved: if (typeof hardwareCtrl !== "undefined") hardwareCtrl.volume = value
                        }
                    }
                }
            }

            // Widget de Reproducción Multimedia
            Rectangle {
                Layout.fillWidth: true
                height: 56
                radius: 12
                color: "#2C2C2E"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    Rectangle {
                        width: 36
                        height: 36
                        radius: 8
                        color: "#FF2D55"
                        Text {
                            anchors.centerIn: parent
                            text: "🎵"
                            font.pixelSize: 16
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text {
                            text: typeof hardwareCtrl !== "undefined" ? hardwareCtrl.mediaTitle : "Música"
                            color: "white"
                            font.bold: true
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }
                        Text {
                            text: typeof hardwareCtrl !== "undefined" ? hardwareCtrl.mediaArtist : "Quartz Audio"
                            color: "#8E8E93"
                            font.pixelSize: 10
                            elide: Text.ElideRight
                        }
                    }

                    // Controles multimedia
                    RowLayout {
                        spacing: 6

                        Text {
                            text: "⏮"
                            color: "white"
                            font.pixelSize: 14
                            MouseArea {
                                anchors.fill: parent
                                onClicked: if (typeof hardwareCtrl !== "undefined") hardwareCtrl.mediaPrevious()
                            }
                        }

                        Text {
                            text: (typeof hardwareCtrl !== "undefined" && hardwareCtrl.isMediaPlaying) ? "⏸" : "▶"
                            color: "white"
                            font.pixelSize: 16
                            MouseArea {
                                anchors.fill: parent
                                onClicked: if (typeof hardwareCtrl !== "undefined") hardwareCtrl.mediaPlayPause()
                            }
                        }

                        Text {
                            text: "⏭"
                            color: "white"
                            font.pixelSize: 14
                            MouseArea {
                                anchors.fill: parent
                                onClicked: if (typeof hardwareCtrl !== "undefined") hardwareCtrl.mediaNext()
                            }
                        }
                    }
                }
            }
        }
    }
}
