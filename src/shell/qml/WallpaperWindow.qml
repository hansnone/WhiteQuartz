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
        cache: true
        source: ""
        visible: status === Image.Ready
    }
}
