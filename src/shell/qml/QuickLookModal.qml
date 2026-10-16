import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    id: quickLookWin
    title: "Quick Look"
    width: 720
    height: 500
    visible: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.visible : false
    color: "transparent"
    flags: Qt.FramelessWindowHint

    x: Screen.desktopAvailableWidth > 0 ? (Screen.desktopAvailableWidth - width) / 2 : 280
    y: Screen.desktopAvailableHeight > 0 ? (Screen.desktopAvailableHeight - height) / 2 : 200

    Rectangle {
        anchors.fill: parent
        radius: 12
        color: "#F8F8F8"
        border.color: "#33000000"
        border.width: 1
        clip: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // 1. Cabecera estilo macOS con botón de cierre
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 38
                color: "#EAEAEA"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 12
                    spacing: 10

                    // Botón de cierre circular
                    Rectangle {
                        width: 14
                        height: 14
                        radius: 7
                        color: closeMouseArea.containsMouse ? "#FF5F56" : "#FF5F56"
                        Text {
                            anchors.centerIn: parent
                            text: "×"
                            color: "#4D0000"
                            font.pixelSize: 11
                            font.bold: true
                            visible: closeMouseArea.containsMouse
                        }
                        MouseArea {
                            id: closeMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (typeof quickLookCtrl !== "undefined") {
                                    quickLookCtrl.closePreview()
                                }
                            }
                        }
                    }

                    Text {
                        Layout.fillWidth: true
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.fileName : ""
                        font.bold: true
                        font.pixelSize: 13
                        elide: Text.ElideMiddle
                        horizontalAlignment: Text.AlignHCenter
                        color: "#1C1C1E"
                    }

                    Text {
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.summary : ""
                        font.pixelSize: 11
                        color: "#8E8E93"
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#D1D1D6"
            }

            // 2. Área de visualización según tipo de contenido
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Vista de Imagen
                Image {
                    anchors.fill: parent
                    anchors.margins: 16
                    visible: typeof quickLookCtrl !== "undefined" && quickLookCtrl.previewType === "Image"
                    source: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.fileUri : ""
                    fillMode: Image.PreserveAspectFit
                }

                // Vista de Texto / Código
                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 16
                    visible: typeof quickLookCtrl !== "undefined" && quickLookCtrl.previewType === "Text"
                    clip: true

                    TextArea {
                        readOnly: true
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.textContent : ""
                        font.family: "Monospace"
                        font.pixelSize: 12
                        color: "#1C1C1E"
                        background: null
                    }
                }

                // Vista de PDF
                ColumnLayout {
                    anchors.centerIn: parent
                    visible: typeof quickLookCtrl !== "undefined" && quickLookCtrl.previewType === "Pdf"
                    spacing: 16

                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 96
                        height: 120
                        radius: 8
                        color: "#FFFFFF"
                        border.color: "#E5E5EA"
                        border.width: 1

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 8
                            Rectangle {
                                Layout.alignment: Qt.AlignHCenter
                                width: 44
                                height: 26
                                radius: 4
                                color: "#FF3B30"
                                Text {
                                    anchors.centerIn: parent
                                    text: "PDF"
                                    font.bold: true
                                    font.pixelSize: 12
                                    color: "#FFFFFF"
                                }
                            }
                            Text {
                                Layout.alignment: Qt.AlignHCenter
                                text: "📑"
                                font.pixelSize: 32
                            }
                        }
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.fileName : ""
                        font.bold: true
                        font.pixelSize: 16
                        color: "#1C1C1E"
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.summary : ""
                        font.pixelSize: 12
                        color: "#8E8E93"
                    }

                    Button {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Abrir con Vista Previa")
                        onClicked: {
                            if (typeof quickLookCtrl !== "undefined") {
                                Qt.openUrlExternally(quickLookCtrl.fileUri)
                                quickLookCtrl.closePreview()
                            }
                        }
                    }
                }

                // Vista de Audio
                ColumnLayout {
                    anchors.centerIn: parent
                    visible: typeof quickLookCtrl !== "undefined" && quickLookCtrl.previewType === "Audio"
                    spacing: 16

                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 96
                        height: 96
                        radius: 48
                        color: "#1C1C1E"
                        Text {
                            anchors.centerIn: parent
                            text: "🎵"
                            font.pixelSize: 42
                        }
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.fileName : ""
                        font.bold: true
                        font.pixelSize: 16
                        color: "#1C1C1E"
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.summary : ""
                        font.pixelSize: 12
                        color: "#8E8E93"
                    }

                    // Barra simulada de reproducción
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 12
                        Text { text: "▶️"; font.pixelSize: 20 }
                        Rectangle {
                            width: 200
                            height: 6
                            radius: 3
                            color: "#D1D1D6"
                            Rectangle {
                                width: 70
                                height: parent.height
                                radius: 3
                                color: "#007AFF"
                            }
                        }
                    }
                }

                // Vista de Vídeo
                ColumnLayout {
                    anchors.centerIn: parent
                    visible: typeof quickLookCtrl !== "undefined" && quickLookCtrl.previewType === "Video"
                    spacing: 16

                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 120
                        height: 80
                        radius: 10
                        color: "#000000"
                        Text {
                            anchors.centerIn: parent
                            text: "🎬"
                            font.pixelSize: 36
                        }
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.fileName : ""
                        font.bold: true
                        font.pixelSize: 16
                        color: "#1C1C1E"
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.summary : ""
                        font.pixelSize: 12
                        color: "#8E8E93"
                    }
                }

                // Vista Genérica / Documentos
                ColumnLayout {
                    anchors.centerIn: parent
                    visible: typeof quickLookCtrl !== "undefined" && quickLookCtrl.previewType !== "Image" && quickLookCtrl.previewType !== "Text" && quickLookCtrl.previewType !== "Pdf" && quickLookCtrl.previewType !== "Audio" && quickLookCtrl.previewType !== "Video"
                    spacing: 12

                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 72
                        height: 72
                        radius: 16
                        color: "#007AFF"
                        Text {
                            anchors.centerIn: parent
                            text: "📄"
                            font.pixelSize: 36
                        }
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.fileName : ""
                        font.bold: true
                        font.pixelSize: 16
                        color: "#1C1C1E"
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: typeof quickLookCtrl !== "undefined" ? quickLookCtrl.summary : ""
                        font.pixelSize: 13
                        color: "#8E8E93"
                    }
                }
            }
        }
    }

    // Atajo Escape para cerrar
    Item {
        focus: quickLookWin.visible
        Keys.onEscapePressed: {
            if (typeof quickLookCtrl !== "undefined") {
                quickLookCtrl.closePreview()
            }
        }
        Keys.onSpacePressed: {
            if (typeof quickLookCtrl !== "undefined") {
                quickLookCtrl.closePreview()
            }
        }
    }
}
