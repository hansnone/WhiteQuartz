import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    width: 980
    height: 640
    visible: true
    title: qsTr("Ajustes del Sistema")

    color: "#F6F6F6"

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Barra lateral izquierda (Sidebar translúcida estilo macOS)
        Rectangle {
            Layout.preferredWidth: 260
            Layout.fillHeight: true
            color: "#EBEBF0"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                // Campo de búsqueda superior
                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Buscar...")
                    onTextChanged: settingsModel.setFilterQuery(text)
                    background: Rectangle {
                        radius: 7
                        color: "#FFFFFF"
                        border.color: "#D1D1D6"
                        border.width: 1
                    }
                }

                // Avatar y cuenta del usuario
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 56
                    radius: 8
                    color: "#FFFFFF"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 10

                        Rectangle {
                            width: 38
                            height: 38
                            radius: 19
                            color: "#007AFF"
                            Text {
                                anchors.centerIn: parent
                                text: "P"
                                color: "white"
                                font.bold: true
                                font.pixelSize: 16
                            }
                        }

                        ColumnLayout {
                            spacing: 2
                            Text {
                                text: qsTr("Pedro")
                                font.bold: true
                                font.pixelSize: 13
                                color: "#1C1C1E"
                            }
                            Text {
                                text: qsTr("Cuenta Local")
                                font.pixelSize: 11
                                color: "#8E8E93"
                            }
                        }
                    }
                }

                // Lista de módulos de configuración
                ListView {
                    id: panesListView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: settingsModel
                    clip: true
                    spacing: 2

                    delegate: Rectangle {
                        width: panesListView.width
                        height: 34
                        radius: 6
                        color: ListView.isCurrentItem ? "#007AFF" : (mouseArea.containsMouse ? "#E1E1E6" : "transparent")

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8
                            spacing: 8

                            Rectangle {
                                width: 20
                                height: 20
                                radius: 4
                                color: "#8E8E93"
                            }

                            Text {
                                Layout.fillWidth: true
                                text: model.title
                                color: ListView.isCurrentItem ? "#FFFFFF" : "#1C1C1E"
                                font.pixelSize: 13
                            }
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: panesListView.currentIndex = index
                        }
                    }
                }
            }
        }

        // Separador vertical
        Rectangle {
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            color: "#D1D1D6"
        }

        // Panel de contenido derecho (Inset Grouped Cards)
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: parent.width - 40
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 16

                Item { height: 10 }

                Text {
                    text: panesListView.currentItem ? panesListView.model.data(panesListView.model.index(panesListView.currentIndex, 0), 258) : qsTr("Ajustes")
                    font.bold: true
                    font.pixelSize: 22
                    color: "#1C1C1E"
                }

                // Tarjeta Inset Grouped
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    radius: 10
                    color: "#FFFFFF"
                    border.color: "#E5E5EA"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            Text {
                                Layout.fillWidth: true
                                text: qsTr("Activar Stage Manager")
                                font.pixelSize: 14
                                color: "#1C1C1E"
                            }
                            Switch {
                                checked: false
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            color: "#E5E5EA"
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Text {
                                Layout.fillWidth: true
                                text: qsTr("Modo de Apariencia Automático")
                                font.pixelSize: 14
                                color: "#1C1C1E"
                            }
                            Switch {
                                checked: true
                            }
                        }
                    }
                }

                Item { height: 20 }
            }
        }
    }
}
