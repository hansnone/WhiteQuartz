import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: switcherWindow
    visible: m_showSwitcher
    width: Math.max(320, rowLayout.implicitWidth + 48)
    height: 140
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool

    x: (Screen.width - width) / 2
    y: (Screen.height - height) / 2

    property bool m_showSwitcher: false
    property int selectedIndex: 0

    property var runningApps: {
        var list = [];
        if (typeof dockCtrl !== "undefined") {
            for (var i = 0; i < dockCtrl.apps.length; ++i) {
                var app = dockCtrl.apps[i];
                if (app.running && !app.isSeparator) {
                    list.push(app);
                }
            }
        }
        return list;
    }

    function toggleSwitcher() {
        if (!m_showSwitcher) {
            selectedIndex = runningApps.length > 1 ? 1 : 0;
            m_showSwitcher = true;
        } else {
            nextApp();
        }
    }

    function nextApp() {
        if (runningApps.length > 0) {
            selectedIndex = (selectedIndex + 1) % runningApps.length;
        }
    }

    function previousApp() {
        if (runningApps.length > 0) {
            selectedIndex = (selectedIndex - 1 + runningApps.length) % runningApps.length;
        }
    }

    function activateSelected() {
        if (runningApps.length > 0 && selectedIndex < runningApps.length) {
            var app = runningApps[selectedIndex];
            if (typeof dockCtrl !== "undefined") {
                dockCtrl.launchApp(app.cmd);
            }
        }
        m_showSwitcher = false;
    }

    Rectangle {
        anchors.fill: parent
        radius: 20
        color: "#CC1C1C1E" // Frosted acrylic oscuro
        border.color: "#33FFFFFF"
        border.width: 1

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 12

            RowLayout {
                id: rowLayout
                spacing: 16
                Layout.alignment: Qt.AlignHCenter

                Repeater {
                    model: runningApps

                    delegate: Rectangle {
                        width: 72
                        height: 72
                        radius: 14
                        color: index === selectedIndex ? "#44FFFFFF" : "transparent"
                        border.color: index === selectedIndex ? "#FFFFFF" : "transparent"
                        border.width: 1.5

                        Behavior on color { ColorAnimation { duration: 100 } }

                        Rectangle {
                            anchors.centerIn: parent
                            width: 52
                            height: 52
                            radius: 12
                            color: {
                                if (modelData.name === "Finder") return "#007AFF"
                                if (modelData.name === "Ajustes del Sistema") return "#8E8E93"
                                if (modelData.name === "Terminal") return "#1C1C1E"
                                return "#34C759"
                            }

                            Text {
                                anchors.centerIn: parent
                                text: modelData.name ? modelData.name.charAt(0).toUpperCase() : "Q"
                                color: "white"
                                font.bold: true
                                font.pixelSize: 24
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                selectedIndex = index;
                                activateSelected();
                            }
                        }
                    }
                }
            }

            Text {
                Layout.alignment: Qt.AlignHCenter
                text: (runningApps.length > 0 && selectedIndex < runningApps.length) ? runningApps[selectedIndex].name : ""
                color: "white"
                font.bold: true
                font.pixelSize: 13
            }
        }
    }
}
