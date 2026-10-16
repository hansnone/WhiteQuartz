import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    id: spotlightWin
    title: "QuartzDE Spotlight"
    width: 680
    height: spotlightCtrl.results.length > 0 ? Math.min(480, 72 + spotlightCtrl.results.length * 52) : 64
    visible: typeof spotlightCtrl !== "undefined" ? spotlightCtrl.visible : false
    color: "transparent"
    flags: Qt.FramelessWindowHint

    x: Screen.desktopAvailableWidth > 0 ? (Screen.desktopAvailableWidth - width) / 2 : 300
    y: Screen.desktopAvailableHeight > 0 ? 180 : 150

    Behavior on height { NumberAnimation { duration: 150 } }

    Rectangle {
        id: panelContainer
        anchors.fill: parent
        radius: 14
        color: "#F0F6F6F6"
        border.color: "#33000000"
        border.width: 1
        clip: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 8

            // 1. Barra de entrada de búsqueda
            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                spacing: 12

                Text {
                    text: "🔍"
                    font.pixelSize: 18
                }

                TextField {
                    id: searchInput
                    Layout.fillWidth: true
                    font.pixelSize: 18
                    placeholderText: qsTr("Búsqueda en Spotlight o cálculo...")
                    text: typeof spotlightCtrl !== "undefined" ? spotlightCtrl.queryText : ""
                    focus: spotlightWin.visible
                    background: null
                    onTextChanged: {
                        if (typeof spotlightCtrl !== "undefined") {
                            spotlightCtrl.queryText = text
                        }
                    }
                    Keys.onEscapePressed: {
                        if (typeof spotlightCtrl !== "undefined") {
                            spotlightCtrl.visible = false
                        }
                    }
                    Keys.onReturnPressed: {
                        if (typeof spotlightCtrl !== "undefined" && spotlightCtrl.results.length > 0) {
                            spotlightCtrl.launchPayload(spotlightCtrl.results[0].payload)
                        }
                    }
                }

                Text {
                    text: "Esc"
                    font.pixelSize: 11
                    color: "#8E8E93"
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -4
                        color: "#1A000000"
                        radius: 4
                        z: -1
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#D1D1D6"
                visible: typeof spotlightCtrl !== "undefined" && spotlightCtrl.results.length > 0
            }

            // 2. Lista de resultados
            ListView {
                id: resultsList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: typeof spotlightCtrl !== "undefined" ? spotlightCtrl.results : []
                visible: count > 0

                delegate: Rectangle {
                    width: resultsList.width
                    height: 48
                    radius: 8
                    color: itemMouseArea.containsMouse ? "#007AFF" : "transparent"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        spacing: 12

                        Rectangle {
                            width: 28
                            height: 28
                            radius: 6
                            color: itemMouseArea.containsMouse ? "#FFFFFF" : (modelData.category === "Calculation" ? "#FF9500" : (modelData.category === "File" ? "#34C759" : "#007AFF"))
                            Text {
                                anchors.centerIn: parent
                                text: modelData.category === "Calculation" ? "=" : (modelData.category === "File" ? "📄" : (modelData.title ? modelData.title.charAt(0) : "A"))
                                color: itemMouseArea.containsMouse ? (modelData.category === "Calculation" ? "#FF9500" : (modelData.category === "File" ? "#34C759" : "#007AFF")) : "#FFFFFF"
                                font.bold: true
                                font.pixelSize: 14
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 1
                            Text {
                                text: modelData.title
                                font.bold: true
                                font.pixelSize: 14
                                color: itemMouseArea.containsMouse ? "#FFFFFF" : "#1C1C1E"
                            }
                            Text {
                                text: modelData.subtitle
                                font.pixelSize: 11
                                Layout.fillWidth: true
                                elide: Text.ElideMiddle
                                color: itemMouseArea.containsMouse ? "#E5E5EA" : "#8E8E93"
                            }
                        }

                        Text {
                            text: modelData.category === "Calculation" ? qsTr("Cálculo") : (modelData.category === "File" ? qsTr("Archivo") : qsTr("App"))
                            font.pixelSize: 11
                            color: itemMouseArea.containsMouse ? "#E5E5EA" : "#8E8E93"
                        }
                    }

                    MouseArea {
                        id: itemMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (typeof spotlightCtrl !== "undefined") {
                                spotlightCtrl.launchPayload(modelData.payload)
                            }
                        }
                    }
                }
            }
        }
    }
}
