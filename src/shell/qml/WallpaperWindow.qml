import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: wallpaperWin
    title: "QuartzDE Wallpaper"
    visible: true
    color: "#1B2838"
    flags: Qt.FramelessWindowHint

    width: (typeof Screen !== "undefined" && Screen.desktopAvailableWidth > 0) ? Screen.desktopAvailableWidth : 1920
    height: (typeof Screen !== "undefined" && Screen.desktopAvailableHeight > 0) ? Screen.desktopAvailableHeight : 1080

    // Fondo degradado fallback estilo macOS Sonoma / Sequoia
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1D2B3A" }
            GradientStop { position: 0.45; color: "#2B4059" }
            GradientStop { position: 0.85; color: "#3A506B" }
            GradientStop { position: 1.0; color: "#182330" }
        }
    }

    // Imagen de fondo opcional con recorte superelíptico/aspect crop
    Image {
        id: bgImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        source: (typeof configCtrl !== "undefined" && configCtrl.wallpaper && configCtrl.wallpaper.length > 0)
                ? (configCtrl.wallpaper.startsWith("/") ? ("file://" + configCtrl.wallpaper) : configCtrl.wallpaper)
                : "file:///usr/share/backgrounds/quartz.png"
        visible: status === Image.Ready
    }

    // Área interactiva de escritorio con clic derecho para menú contextual
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: (mouse) => {
            if (mouse.button === Qt.RightButton) {
                desktopMenu.x = mouse.x
                desktopMenu.y = mouse.y
                desktopMenu.open()
            }
        }
    }

    Menu {
        id: desktopMenu
        MenuItem { text: qsTr("Nueva carpeta"); onTriggered: if (typeof dockCtrl !== "undefined") dockCtrl.launchApp("quartz-finder") }
        MenuItem { text: qsTr("Obtener información") }
        MenuSeparator {}
        MenuItem { text: qsTr("Cambiar fondo de pantalla..."); onTriggered: if (typeof dockCtrl !== "undefined") dockCtrl.launchApp("quartz-settings") }
        MenuSeparator {}
        MenuItem { text: qsTr("Usar pilas") }
        MenuItem { text: qsTr("Ordenar por nombre") }
    }

    // Cuadrícula de iconos de escritorio (alineación superior derecha estilo macOS)
    Column {
        anchors.top: parent.top
        anchors.topMargin: 36
        anchors.right: parent.right
        anchors.rightMargin: 20
        spacing: 16

        Repeater {
            model: [
                { name: "Macintosh HD", icon: "💽", cmd: "quartz-finder /" },
                { name: "Documentos", icon: "📁", cmd: "quartz-finder ~/Documentos" },
                { name: "Descargas", icon: "📥", cmd: "quartz-finder ~/Descargas" }
            ]

            delegate: Rectangle {
                width: 74
                height: 84
                radius: 6
                color: itemMouse.containsMouse ? "#44000000" : "transparent"

                Column {
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: modelData.icon
                        font.pixelSize: 38
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: modelData.name
                        color: "white"
                        font.pixelSize: 11
                        font.bold: true
                        style: Text.Outline
                        styleColor: "#99000000"
                    }
                }

                MouseArea {
                    id: itemMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onDoubleClicked: {
                        if (typeof dockCtrl !== "undefined") {
                            dockCtrl.launchApp(modelData.cmd)
                        }
                    }
                }
            }
        }
    }
}
