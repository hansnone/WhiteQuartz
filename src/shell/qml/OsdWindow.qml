import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: osdWindow
    visible: opacity > 0
    opacity: 0.0
    width: 200
    height: 200
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool

    // Centrado exacto en pantalla
    x: (Screen.width - width) / 2
    y: (Screen.height - height) / 2

    property string osdIcon: "🔊"
    property int osdLevel: 50
    property string osdLabel: "Volumen"

    Behavior on opacity {
        NumberAnimation { duration: 200; easing.type: Easing.OutQuad }
    }

    Timer {
        id: hideTimer
        interval: 1600
        repeat: false
        onTriggered: osdWindow.opacity = 0.0
    }

    Connections {
        target: typeof hardwareCtrl !== "undefined" ? hardwareCtrl : null
        function onOsdTriggered(icon, level, label) {
            osdWindow.osdLevel = level;
            osdWindow.osdLabel = label;
            if (label === "Brillo") {
                osdWindow.osdIcon = "🔆";
            } else {
                osdWindow.osdIcon = (level === 0) ? "🔇" : "🔊";
            }
            osdWindow.opacity = 1.0;
            hideTimer.restart();
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: 24
        color: "#D91C1C1E" // Frosted Bezel macOS
        border.color: "#33FFFFFF"
        border.width: 1

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 16
            width: parent.width - 40

            Text {
                Layout.alignment: Qt.AlignHCenter
                text: osdWindow.osdIcon
                font.pixelSize: 52
                color: "white"
            }

            // Barra de progreso continua con esquinas redondeadas
            Rectangle {
                Layout.fillWidth: true
                height: 7
                radius: 3.5
                color: "#3A3A3C"

                Rectangle {
                    width: parent.width * (Math.max(0, Math.min(100, osdWindow.osdLevel)) / 100.0)
                    height: parent.height
                    radius: 3.5
                    color: "#FFFFFF"
                }
            }

            Text {
                Layout.alignment: Qt.AlignHCenter
                text: osdWindow.osdLabel
                font.pixelSize: 11
                color: "#8E8E93"
            }
        }
    }
}
