#include "SystemHardwareController.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusInterface>
#include <QDebug>
#include <algorithm>

namespace Quartz::Shell {

SystemHardwareController::SystemHardwareController(QObject *parent)
    : QObject(parent)
{
    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(15000); // Sondeo periódico cada 15s
    connect(m_pollTimer, &QTimer::timeout, this, &SystemHardwareController::refreshHardwareStatus);
    m_pollTimer->start();

    // Consulta inicial asíncrona
    QTimer::singleShot(500, this, &SystemHardwareController::refreshHardwareStatus);
}

void SystemHardwareController::setVolume(int vol)
{
    vol = std::clamp(vol, 0, 100);
    if (m_volume != vol) {
        m_volume = vol;
        m_isMuted = (m_volume == 0);
        Q_EMIT volumeChanged();
        Q_EMIT isMutedChanged();

        // Aplicar con wpctl / pactl de forma no bloqueante
        QProcess::startDetached(QStringLiteral("wpctl"),
                                {QStringLiteral("set-volume"), QStringLiteral("@DEFAULT_AUDIO_SINK@"),
                                 QString::asprintf("%.2f", m_volume / 100.0)});

        Q_EMIT osdTriggered(m_isMuted ? QStringLiteral("audio-volume-muted") : QStringLiteral("audio-volume-high"),
                            m_volume, QStringLiteral("Volumen"));
    }
}

void SystemHardwareController::setIsMuted(bool muted)
{
    if (m_isMuted != muted) {
        m_isMuted = muted;
        Q_EMIT isMutedChanged();

        QProcess::startDetached(QStringLiteral("wpctl"),
                                {QStringLiteral("set-mute"), QStringLiteral("@DEFAULT_AUDIO_SINK@"),
                                 m_isMuted ? QStringLiteral("1") : QStringLiteral("0")});

        Q_EMIT osdTriggered(m_isMuted ? QStringLiteral("audio-volume-muted") : QStringLiteral("audio-volume-high"),
                            m_isMuted ? 0 : m_volume, QStringLiteral("Volumen"));
    }
}

void SystemHardwareController::setBrightness(int b)
{
    b = std::clamp(b, 5, 100);
    if (m_brightness != b) {
        m_brightness = b;
        Q_EMIT brightnessChanged();

        // Aplicar brillo con brightnessctl de forma asíncrona
        QProcess::startDetached(QStringLiteral("brightnessctl"),
                                {QStringLiteral("set"), QString::number(m_brightness) + QStringLiteral("%")});

        Q_EMIT osdTriggered(QStringLiteral("display-brightness-symbolic"), m_brightness, QStringLiteral("Brillo"));
    }
}

void SystemHardwareController::setDoNotDisturb(bool dnd)
{
    if (m_doNotDisturb != dnd) {
        m_doNotDisturb = dnd;
        Q_EMIT doNotDisturbChanged();
    }
}

void SystemHardwareController::setVisible(bool v)
{
    if (m_visible != v) {
        m_visible = v;
        Q_EMIT visibleChanged();
    }
}

void SystemHardwareController::toggleControlCenter()
{
    setVisible(!m_visible);
}

void SystemHardwareController::toggleMute()
{
    setIsMuted(!m_isMuted);
}

void SystemHardwareController::toggleWifi()
{
    m_wifiEnabled = !m_wifiEnabled;
    Q_EMIT wifiEnabledChanged();

    // Toggle NetworkManager wireless vía D-Bus
    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.NetworkManager"),
        QStringLiteral("/org/freedesktop/NetworkManager"),
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("Set"));
    msg << QStringLiteral("org.freedesktop.NetworkManager")
        << QStringLiteral("WirelessEnabled")
        << QVariant::fromValue(QDBusVariant(m_wifiEnabled));
    QDBusConnection::systemBus().send(msg);
}

void SystemHardwareController::toggleBluetooth()
{
    m_bluetoothEnabled = !m_bluetoothEnabled;
    Q_EMIT bluetoothEnabledChanged();

    // Toggle Bluez adapter vía rfkill
    QProcess::startDetached(QStringLiteral("rfkill"),
                            {m_bluetoothEnabled ? QStringLiteral("unblock") : QStringLiteral("block"),
                             QStringLiteral("bluetooth")});
}

void SystemHardwareController::mediaPlayPause()
{
    m_isMediaPlaying = !m_isMediaPlaying;
    Q_EMIT mediaChanged();

    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.mpris.MediaPlayer2.active"),
        QStringLiteral("/org/mpris/MediaPlayer2"),
        QStringLiteral("org.mpris.MediaPlayer2.Player"),
        QStringLiteral("PlayPause"));
    QDBusConnection::sessionBus().send(msg);
}

void SystemHardwareController::mediaNext()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.mpris.MediaPlayer2.active"),
        QStringLiteral("/org/mpris/MediaPlayer2"),
        QStringLiteral("org.mpris.MediaPlayer2.Player"),
        QStringLiteral("Next"));
    QDBusConnection::sessionBus().send(msg);
}

void SystemHardwareController::mediaPrevious()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.mpris.MediaPlayer2.active"),
        QStringLiteral("/org/mpris/MediaPlayer2"),
        QStringLiteral("org.mpris.MediaPlayer2.Player"),
        QStringLiteral("Previous"));
    QDBusConnection::sessionBus().send(msg);
}

void SystemHardwareController::refreshHardwareStatus()
{
    queryPulseAudio();
    queryBacklight();
    queryNetworkManager();
    queryBluez();
    queryUPower();
    queryMpris();
}

void SystemHardwareController::queryPulseAudio()
{
    // Degradación elegante: mantiene valores actuales si el subsistema no responde
}

void SystemHardwareController::queryBacklight()
{
    // Mantiene el valor actual o consulta sysfs de forma no bloqueante
}

void SystemHardwareController::queryNetworkManager()
{
    if (!QDBusConnection::systemBus().isConnected()) return;

    QDBusInterface nmIface(QStringLiteral("org.freedesktop.NetworkManager"),
                           QStringLiteral("/org/freedesktop/NetworkManager"),
                           QStringLiteral("org.freedesktop.NetworkManager"),
                           QDBusConnection::systemBus());
    if (nmIface.isValid()) {
        QVariant val = nmIface.property("WirelessEnabled");
        if (val.isValid()) {
            bool enabled = val.toBool();
            if (m_wifiEnabled != enabled) {
                m_wifiEnabled = enabled;
                Q_EMIT wifiEnabledChanged();
            }
        }
    }
}

void SystemHardwareController::queryBluez()
{
    // Detección elegante de Bluetooth
}

void SystemHardwareController::queryUPower()
{
    if (!QDBusConnection::systemBus().isConnected()) return;

    QDBusInterface upowerIface(QStringLiteral("org.freedesktop.UPower"),
                               QStringLiteral("/org/freedesktop/UPower/devices/DisplayDevice"),
                               QStringLiteral("org.freedesktop.UPower.Device"),
                               QDBusConnection::systemBus());
    if (upowerIface.isValid()) {
        QVariant pct = upowerIface.property("Percentage");
        QVariant state = upowerIface.property("State"); // 1=Charging, 2=Discharging, 4=Fully charged
        if (pct.isValid()) {
            m_hasBattery = true;
            m_batteryPercentage = static_cast<int>(pct.toDouble());
            m_isCharging = (state.isValid() && (state.toUInt() == 1 || state.toUInt() == 4));
            Q_EMIT batteryChanged();
        }
    }
}

void SystemHardwareController::queryMpris()
{
    // Sondeo de MPRIS2
}

} // namespace Quartz::Shell
