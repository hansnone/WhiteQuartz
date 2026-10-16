import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: lockScreenRoot
    width: Screen.width > 0 ? Screen.width : 1920
    height: Screen.height > 0 ? Screen.height : 1080

    // Fondo con el wallpaper oficial de Quartz
    Image {
        id: bgImage
        anchors.fill: parent
        source: "file:///usr/share/backgrounds/quartz.png"
        fillMode: Image.PreserveAspectCrop
    }

    Rectangle {
        anchors.fill: parent
        color: "#40000000" // Capa sutil de oscurecimiento
    }

    // Reloj grande superior estilo macOS Sonoma / Sequoia
    ColumnLayout {
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.12
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 4

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: Qt.formatDate(new Date(), "dddd, d 'de' MMMM")
            color: "#EBEBF5"
            font.pixelSize: 20
            font.weight: Font.Medium
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: Qt.formatTime(new Date(), "hh:mm")
            color: "#FFFFFF"
            font.pixelSize: 86
            font.bold: true
        }
    }

    // Tarjeta central de inicio de sesión / desbloqueo
    ColumnLayout {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 60
        spacing: 16

        // Avatar del usuario
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 80
            height: 80
            radius: 40
            color: "#007AFF"
            border.color: "#FFFFFF"
            border.width: 2

            Text {
                anchors.centerIn: parent
                text: "P"
                color: "white"
                font.bold: true
                font.pixelSize: 34
            }
        }

        // Nombre de usuario
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Pedro")
            color: "#FFFFFF"
            font.pixelSize: 17
            font.bold: true
        }

        // Campo de contraseña estilo cápsula macOS
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 240
            height: 38
            radius: 19
            color: "#66000000"
            border.color: pwInput.activeFocus ? "#007AFF" : "#44FFFFFF"
            border.width: 1.5

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 6
                spacing: 6

                TextInput {
                    id: pwInput
                    Layout.fillWidth: true
                    echoMode: TextInput.Password
                    color: "white"
                    font.pixelSize: 14
                    focus: true
                    clip: true

                    Text {
                        anchors.fill: parent
                        text: qsTr("Ingresa tu contraseña")
                        color: "#99FFFFFF"
                        font.pixelSize: 13
                        visible: !pwInput.text && !pwInput.activeFocus
                    }

                    onAccepted: {
                        if (typeof authenticator !== "undefined") {
                            authenticator.tryUnlock(pwInput.text)
                        }
                    }
                }

                // Botón flecha de envío
                Rectangle {
                    width: 26
                    height: 26
                    radius: 13
                    color: pwInput.text.length > 0 ? "#007AFF" : "#33FFFFFF"

                    Text {
                        anchors.centerIn: parent
                        text: "➔"
                        color: "white"
                        font.pixelSize: 12
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (typeof authenticator !== "undefined") {
                                authenticator.tryUnlock(pwInput.text)
                            }
                        }
                    }
                }
            }
        }
    }

    // Botones de energía inferiores (Reposo, Reiniciar, Apagar)
    RowLayout {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 32
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 36

        Repeater {
            model: [
                { label: qsTr("Reposo"), icon: "💤", action: "sleep" },
                { label: qsTr("Reiniciar"), icon: "🔄", action: "reboot" },
                { label: qsTr("Apagar"), icon: "⏻", action: "shutdown" }
            ]

            delegate: ColumnLayout {
                spacing: 6

                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 44
                    height: 44
                    radius: 22
                    color: pMouse.containsMouse ? "#66FFFFFF" : "#33000000"
                    border.color: "#33FFFFFF"
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: modelData.icon
                        font.pixelSize: 18
                    }

                    MouseArea {
                        id: pMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                    }
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: modelData.label
                    color: "#D1D1D6"
                    font.pixelSize: 12
                }
            }
        }
    }
}
