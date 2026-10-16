import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    id: launchpadWin
    title: "Launchpad"
    width: Screen.desktopAvailableWidth > 0 ? Screen.desktopAvailableWidth : 1920
    height: Screen.desktopAvailableHeight > 0 ? Screen.desktopAvailableHeight : 1080
    visible: typeof launchpadCtrl !== "undefined" ? launchpadCtrl.visible : false
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    // Fondo translúcido oscuro con desenfoque simulado
    Rectangle {
        id: bgOverlay
        anchors.fill: parent
        color: "#B3000000"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (typeof launchpadCtrl !== "undefined") {
                    launchpadCtrl.visible = false
                }
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.topMargin: 48
            anchors.bottomMargin: 48
            anchors.leftMargin: 80
            anchors.rightMargin: 80
            spacing: 24

            // 1. Barra de Búsqueda estilo macOS
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 260
                Layout.preferredHeight: 32
                radius: 8
                color: "#40FFFFFF"
                border.color: "#33FFFFFF"
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    spacing: 6

                    Text {
                        text: "🔍"
                        font.pixelSize: 12
                        color: "#FFFFFF"
                    }

                    TextField {
                        id: searchBox
                        Layout.fillWidth: true
                        font.pixelSize: 13
                        color: "#FFFFFF"
                        placeholderText: qsTr("Buscar")
                        placeholderTextColor: "#99FFFFFF"
                        background: null
                        focus: launchpadWin.visible
                        text: typeof launchpadCtrl !== "undefined" ? launchpadCtrl.filterText : ""
                        onTextChanged: {
                            if (typeof launchpadCtrl !== "undefined") {
                                launchpadCtrl.filterText = text
                            }
                        }
                        Keys.onEscapePressed: {
                            if (typeof launchpadCtrl !== "undefined") {
                                launchpadCtrl.visible = false
                            }
                        }
                    }

                    Text {
                        text: "✕"
                        font.pixelSize: 11
                        color: "#B3FFFFFF"
                        visible: searchBox.text.length > 0
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: searchBox.text = ""
                        }
                    }
                }
            }

            // 2. Cuadrícula de Aplicaciones
            GridView {
                id: appsGrid
                Layout.fillWidth: true
                Layout.fillHeight: true
                cellWidth: 140
                cellHeight: 140
                clip: true
                model: typeof launchpadCtrl !== "undefined" ? launchpadCtrl.apps : []

                delegate: Item {
                    width: appsGrid.cellWidth
                    height: appsGrid.cellHeight

                    Rectangle {
                        id: appCard
                        anchors.fill: parent
                        anchors.margins: 8
                        radius: 14
                        color: hoverArea.containsMouse ? "#26FFFFFF" : "transparent"
                        scale: hoverArea.containsMouse ? 1.05 : 1.0
                        Behavior on scale { NumberAnimation { duration: 120 } }
                        Behavior on color { ColorAnimation { duration: 120 } }

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 8

                            // Icono estilizado
                            Rectangle {
                                Layout.alignment: Qt.AlignHCenter
                                Layout.preferredWidth: 64
                                Layout.preferredHeight: 64
                                radius: 14
                                color: "#2B2B2E"
                                border.color: "#33FFFFFF"
                                border.width: 1

                                Image {
                                    anchors.centerIn: parent
                                    width: 48
                                    height: 48
                                    source: modelData.icon ? ("image://icon/" + modelData.icon) : ""
                                    sourceSize: Qt.size(48, 48)
                                    fillMode: Image.PreserveAspectFit
                                    visible: status === Image.Ready
                                }

                                Text {
                                    anchors.centerIn: parent
                                    text: modelData.name.length > 0 ? modelData.name.substring(0, 1).toUpperCase() : "A"
                                    font.pixelSize: 26
                                    font.bold: true
                                    color: "#FFFFFF"
                                    visible: true
                                }
                            }

                            // Nombre de la App
                            Text {
                                Layout.alignment: Qt.AlignHCenter
                                Layout.maximumWidth: 120
                                text: modelData.name
                                font.pixelSize: 13
                                font.weight: Font.Medium
                                color: "#FFFFFF"
                                elide: Text.ElideRight
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                        MouseArea {
                            id: hoverArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (typeof launchpadCtrl !== "undefined") {
                                    launchpadCtrl.launchApp(modelData.exec)
                                }
                            }
                        }
                    }
                }
            }

            // 3. Indicador de Páginas / Contador de Apps
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 8

                Text {
                    text: qsTr("%1 aplicaciones disponibles").arg(typeof launchpadCtrl !== "undefined" ? launchpadCtrl.apps.length : 0)
                    font.pixelSize: 12
                    color: "#80FFFFFF"
                }
            }
        }
    }
}
