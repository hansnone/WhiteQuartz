import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: sddmRoot
    width: 1920
    height: 1080

    Image {
        id: bgImage
        anchors.fill: parent
        source: config.background || "file:///usr/share/backgrounds/quartz.png"
        fillMode: Image.PreserveAspectCrop
    }

    Rectangle {
        anchors.fill: parent
        color: "#4D000000"
    }

    // Cabecera con Reloj y Selección de Sesión
    RowLayout {
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 28

        Text {
            text: Qt.formatTime(new Date(), "hh:mm") + "  •  " + Qt.formatDate(new Date(), "dddd, d 'de' MMMM")
            color: "#FFFFFF"
            font.pixelSize: 14
            font.bold: true
        }

        Item { Layout.fillWidth: true }

        // Selector de sesión
        ComboBox {
            id: sessionSelect
            model: (typeof sessionModel !== "undefined") ? sessionModel : ["QuartzDE (Wayland)"]
            textRole: "name"
            currentIndex: (typeof sessionModel !== "undefined") ? sessionModel.lastIndex : 0
        }
    }

    // Tarjeta central de inicio de sesión
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 88
            height: 88
            radius: 44
            color: "#007AFF"
            border.color: "#FFFFFF"
            border.width: 2.5

            Text {
                anchors.centerIn: parent
                text: "P"
                color: "white"
                font.bold: true
                font.pixelSize: 36
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: (typeof userModel !== "undefined" && userModel.lastUser) ? userModel.lastUser : qsTr("Pedro")
            color: "#FFFFFF"
            font.pixelSize: 18
            font.bold: true
        }

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 250
            height: 38
            radius: 19
            color: "#66000000"
            border.color: pwInput.activeFocus ? "#007AFF" : "#55FFFFFF"
            border.width: 1.5

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 8
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
                        if (typeof sddm !== "undefined") {
                            sddm.login(userModel.lastUser || "pedro", pwInput.text, sessionSelect.currentIndex)
                        }
                    }
                }

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
                            if (typeof sddm !== "undefined") {
                                sddm.login(userModel.lastUser || "pedro", pwInput.text, sessionSelect.currentIndex)
                            }
                        }
                    }
                }
            }
        }
    }

    // Botones de energía inferiores
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
                        onClicked: {
                            if (typeof sddm !== "undefined") {
                                if (modelData.action === "sleep") sddm.suspend();
                                else if (modelData.action === "reboot") sddm.reboot();
                                else if (modelData.action === "shutdown") sddm.powerOff();
                            }
                        }
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
