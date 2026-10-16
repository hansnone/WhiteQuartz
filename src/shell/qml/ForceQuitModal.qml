import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: forceQuitWin
    visible: m_visible
    width: 380
    height: 340
    color: "transparent"
    flags: Qt.Dialog | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    x: (Screen.width - width) / 2
    y: (Screen.height - height) / 2

    property bool m_visible: false
    property int selectedIndex: -1

    function showDialog() {
        selectedIndex = -1;
        m_visible = true;
    }

    function hideDialog() {
        m_visible = false;
    }

    property var userApps: {
        var list = [];
        if (typeof dockCtrl !== "undefined") {
            for (var i = 0; i < dockCtrl.apps.length; ++i) {
                var app = dockCtrl.apps[i];
                // Excluir componentes protegidos del sistema
                if (app.running && !app.isSeparator && app.cmd !== "quartz-session" && app.cmd !== "kwin_wayland" && app.cmd !== "quartz-shell") {
                    list.push(app);
                }
            }
        }
        return list;
    }

    Rectangle {
        anchors.fill: parent
        radius: 16
        color: "#F6F6F6"
        border.color: "#D1D1D6"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 12

            Text {
                text: qsTr("Forzar salida de las aplicaciones")
                font.bold: true
                font.pixelSize: 15
                color: "#1C1C1E"
            }

            Text {
                text: qsTr("Si una aplicación no responde durante un tiempo, selecciónala y haz clic en Forzar salida.")
                font.pixelSize: 11
                color: "#8E8E93"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            // Lista con borde
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 8
                color: "#FFFFFF"
                border.color: "#E5E5EA"

                ListView {
                    id: appList
                    anchors.fill: parent
                    anchors.margins: 4
                    clip: true
                    model: userApps

                    delegate: Rectangle {
                        width: appList.width
                        height: 32
                        radius: 6
                        color: index === selectedIndex ? "#007AFF" : "transparent"

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8
                            spacing: 8

                            Rectangle {
                                width: 18
                                height: 18
                                radius: 4
                                color: "#34C759"
                                Text {
                                    anchors.centerIn: parent
                                    text: modelData.name ? modelData.name.charAt(0).toUpperCase() : "A"
                                    color: "white"
                                    font.bold: true
                                    font.pixelSize: 10
                                }
                            }

                            Text {
                                text: modelData.name
                                color: index === selectedIndex ? "#FFFFFF" : "#1C1C1E"
                                font.pixelSize: 12
                                Layout.fillWidth: true
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: selectedIndex = index
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Item { Layout.fillWidth: true }

                Button {
                    text: qsTr("Cancelar")
                    onClicked: hideDialog()
                }

                Button {
                    text: qsTr("Forzar salida")
                    enabled: selectedIndex >= 0 && selectedIndex < userApps.length
                    onClicked: {
                        if (selectedIndex >= 0 && selectedIndex < userApps.length) {
                            var target = userApps[selectedIndex];
                            if (typeof dockCtrl !== "undefined") {
                                dockCtrl.forceQuitApp(target.cmd);
                            }
                            hideDialog();
                        }
                    }
                }
            }
        }
    }
}
