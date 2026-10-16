import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    id: dockWin
    title: "QuartzDE Dock"
    height: stackPopup.visible ? 440 : 88
    width: Math.max(dockContainer.width + 64, 420)
    visible: true
    color: "transparent"
    flags: Qt.FramelessWindowHint

    // Posición centrada en la base de la pantalla
    x: Screen.desktopAvailableWidth > 0 ? (Screen.desktopAvailableWidth - width) / 2 : 100
    y: Screen.desktopAvailableHeight > 0 ? (Screen.desktopAvailableHeight - height) : 700

    // Área para cerrar el Stack al pulsar fuera de él
    MouseArea {
        anchors.fill: parent
        visible: stackPopup.visible
        z: 50
        onClicked: stackPopup.closeStack()
    }

    // Grid Stack Popup para "Descargas" estilo macOS Sonoma
    Rectangle {
        id: stackPopup
        visible: false
        z: 60
        anchors.bottom: dockContainer.top
        anchors.bottomMargin: 12
        x: Math.max(12, Math.min(dockWin.width - width - 12, stackTargetX - width / 2))
        width: 320
        height: 320
        radius: 16
        color: "#F2F5F5F7"
        border.color: "#33000000"
        border.width: 1

        property real stackTargetX: 0
        property var stackItems: []

        function openStack(targetX) {
            stackTargetX = targetX
            stackItems = dockCtrl.listStackDirectory("~/Descargas")
            visible = true
        }

        function closeStack() {
            visible = false
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 8

            // Cabecera del Stack
            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: qsTr("Descargas")
                    font.bold: true
                    font.pixelSize: 13
                    color: "#1C1C1E"
                }
                Item { Layout.fillWidth: true }
                Text {
                    text: stackPopup.stackItems.length > 0 ? (stackPopup.stackItems.length + qsTr(" ítems")) : ""
                    font.pixelSize: 11
                    color: "#8E8E93"
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#1A000000"
            }

            // Contenido en rejilla
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    anchors.centerIn: parent
                    visible: stackPopup.stackItems.length === 0
                    text: qsTr("No hay descargas recientes")
                    font.pixelSize: 12
                    color: "#8E8E93"
                }

                Grid {
                    id: stackGrid
                    anchors.fill: parent
                    columns: 3
                    spacing: 8
                    visible: stackPopup.stackItems.length > 0

                    Repeater {
                        model: stackPopup.stackItems
                        delegate: Rectangle {
                            width: (stackGrid.width - (stackGrid.spacing * 2)) / 3
                            height: 70
                            radius: 8
                            color: itemHover.hovered ? "#14007AFF" : "transparent"

                            HoverHandler {
                                id: itemHover
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    stackPopup.closeStack()
                                    if (modelData.isDir) {
                                        dockCtrl.launchApp("quartz-finder " + modelData.path)
                                    } else {
                                        dockCtrl.launchApp("xdg-open " + modelData.path)
                                    }
                                }
                            }

                            Column {
                                anchors.centerIn: parent
                                spacing: 4
                                width: parent.width - 4

                                Rectangle {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: 34
                                    height: 34
                                    radius: 8
                                    color: modelData.isDir ? "#007AFF" : "#8E8E93"

                                    Text {
                                        anchors.centerIn: parent
                                        text: {
                                            if (modelData.isDir) return "📁"
                                            if (modelData.icon === "image-x-generic") return "🖼"
                                            if (modelData.icon === "audio-x-generic") return "🎵"
                                            if (modelData.icon === "video-x-generic") return "🎬"
                                            if (modelData.icon === "application-pdf") return "📄"
                                            if (modelData.icon === "package-x-generic") return "📦"
                                            return "📝"
                                        }
                                        font.pixelSize: 16
                                    }
                                }

                                Text {
                                    width: parent.width
                                    text: modelData.name
                                    font.pixelSize: 10
                                    color: "#1C1C1E"
                                    horizontalAlignment: Text.AlignHCenter
                                    elide: Text.ElideMiddle
                                    maximumLineCount: 1
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#1A000000"
            }

            // Pie: Abrir en el Finder
            Rectangle {
                Layout.fillWidth: true
                height: 24
                radius: 6
                color: footerHover.hovered ? "#14007AFF" : "transparent"

                HoverHandler {
                    id: footerHover
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        stackPopup.closeStack()
                        dockCtrl.launchApp("quartz-finder ~/Descargas")
                    }
                }

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 4
                    Text {
                        text: qsTr("Abrir en el Finder")
                        font.pixelSize: 11
                        color: "#007AFF"
                    }
                    Text {
                        text: "›"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#007AFF"
                    }
                }
            }
        }
    }

    Rectangle {
        id: dockContainer
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
        anchors.horizontalCenter: parent.horizontalCenter
        height: 64
        width: dockRow.implicitWidth + 24
        radius: 18
        color: "#CCFFFFFF"
        border.color: "#33FFFFFF"
        border.width: 1
        z: 10

        property real mouseXPos: -1000
        property bool isHovered: false

        HoverHandler {
            id: dockHover
            onHoveredChanged: {
                dockContainer.isHovered = dockHover.hovered
                if (!dockHover.hovered) {
                    dockContainer.mouseXPos = -1000
                }
            }
            onPointChanged: {
                if (dockHover.hovered) {
                    dockContainer.mouseXPos = dockHover.point.position.x
                }
            }
        }

        Row {
            id: dockRow
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8

            Repeater {
                model: dockCtrl.apps
                delegate: Item {
                    id: dockItem
                    property real baseSize: dockCtrl.baseIconSize
                    property real targetIconSize: {
                        if (modelData.isSeparator) return 8
                        if (!dockContainer.isHovered || dockContainer.mouseXPos < 0) {
                            return baseSize
                        }
                        var itemCenterX = dockRow.x + dockItem.x + baseSize / 2
                        var dist = Math.abs(dockContainer.mouseXPos - itemCenterX)
                        return dockCtrl.calculateGaussianMagnification(dist, 55.0)
                    }

                    width: modelData.isSeparator ? 8 : (targetIconSize + 2)
                    height: 52

                    Behavior on width {
                        NumberAnimation { duration: 60; easing.type: Easing.OutQuad }
                    }

                    // Separador vertical fino
                    Rectangle {
                        visible: modelData.isSeparator
                        anchors.centerIn: parent
                        width: 1
                        height: 38
                        color: "#B0B0B5"
                    }

                    // Icono de app
                    Rectangle {
                        id: appIcon
                        visible: !modelData.isSeparator
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 4
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: targetIconSize
                        height: targetIconSize
                        radius: Math.min(18, Math.round(width * 0.23))
                        color: {
                            if (modelData.name === "Finder") return "#007AFF"
                            if (modelData.name === "Launchpad") return "#5856D6"
                            if (modelData.name === "Ajustes del Sistema") return "#8E8E93"
                            if (modelData.name === "Terminal") return "#1C1C1E"
                            if (modelData.name === "Descargas") return "#007AFF"
                            if (modelData.name === "Papelera") return "#8E8E93"
                            return "#34C759"
                        }

                        Behavior on width {
                            NumberAnimation { duration: 60; easing.type: Easing.OutQuad }
                        }
                        Behavior on height {
                            NumberAnimation { duration: 60; easing.type: Easing.OutQuad }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: {
                                if (modelData.name === "Finder") return "Finder"
                                if (modelData.name === "Launchpad") return "㗊"
                                if (modelData.name === "Ajustes del Sistema") return "⚙"
                                if (modelData.name === "Terminal") return ">_"
                                if (modelData.name === "Descargas") return "􀈄"
                                if (modelData.name === "Papelera") return (dockCtrl.trashIcon === "user-trash-full" ? "🗑*" : "🗑")
                                return modelData.name ? modelData.name.charAt(0).toUpperCase() : ""
                            }
                            color: "white"
                            font.bold: true
                            font.pixelSize: {
                                if (modelData.name === "Finder") return Math.max(10, Math.round(appIcon.width * 0.22))
                                return Math.max(14, Math.round(appIcon.width * 0.38))
                            }
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (modelData.name === "Descargas") {
                                    var itemCenterX = dockContainer.x + dockRow.x + dockItem.x + dockItem.width / 2
                                    if (stackPopup.visible) {
                                        stackPopup.closeStack()
                                    } else {
                                        stackPopup.openStack(itemCenterX)
                                    }
                                    return
                                }
                                if (modelData.cmd === "quartz-spotlight") {
                                    if (typeof spotlightCtrl !== "undefined") {
                                        spotlightCtrl.toggle()
                                        return
                                    }
                                }
                                dockCtrl.launchApp(modelData.cmd)
                            }
                        }

                        // Tooltip estilo macOS
                        Rectangle {
                            id: tooltipPill
                            visible: mouseArea.containsMouse && !modelData.isSeparator && !stackPopup.visible
                            anchors.bottom: appIcon.top
                            anchors.bottomMargin: 8
                            anchors.horizontalCenter: appIcon.horizontalCenter
                            width: tooltipText.implicitWidth + 16
                            height: 22
                            radius: 6
                            color: "#D9000000"
                            border.color: "#33FFFFFF"
                            border.width: 1
                            z: 100

                            Text {
                                id: tooltipText
                                anchors.centerIn: parent
                                text: modelData.name || ""
                                color: "white"
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }

                        // Animación de rebote parabólico
                        SequentialAnimation {
                            id: bounceAnim
                            loops: 2
                            NumberAnimation {
                                target: appIcon
                                property: "anchors.bottomMargin"
                                to: 4 + 20
                                duration: 180
                                easing.type: Easing.OutQuad
                            }
                            NumberAnimation {
                                target: appIcon
                                property: "anchors.bottomMargin"
                                to: 4
                                duration: 180
                                easing.type: Easing.InQuad
                            }
                        }

                        Connections {
                            target: dockCtrl
                            function onAppBouncingRequested(cmd) {
                                if (modelData.cmd === cmd) {
                                    bounceAnim.restart()
                                }
                            }
                        }
                    }

                    // Indicador de app activa: punto luminoso de 4pt
                    Rectangle {
                        visible: !modelData.isSeparator && modelData.running
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 4
                        height: 4
                        radius: 2
                        color: "#1C1C1E"
                    }
                }
            }
        }
    }
}
