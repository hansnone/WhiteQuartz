#pragma once

#include <QObject>
#include <QString>
#include <QProcess>
#include <QTimer>

namespace Quartz::Shell {

class SystemHardwareController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool isMuted READ isMuted WRITE setIsMuted NOTIFY isMutedChanged)
    Q_PROPERTY(int brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(bool wifiEnabled READ wifiEnabled NOTIFY wifiEnabledChanged)
    Q_PROPERTY(QString wifiSsid READ wifiSsid NOTIFY wifiSsidChanged)
    Q_PROPERTY(bool bluetoothEnabled READ bluetoothEnabled NOTIFY bluetoothEnabledChanged)
    Q_PROPERTY(bool hasBattery READ hasBattery NOTIFY batteryChanged)
    Q_PROPERTY(int batteryPercentage READ batteryPercentage NOTIFY batteryChanged)
    Q_PROPERTY(bool isCharging READ isCharging NOTIFY batteryChanged)
    Q_PROPERTY(bool doNotDisturb READ doNotDisturb WRITE setDoNotDisturb NOTIFY doNotDisturbChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaChanged)
    Q_PROPERTY(QString mediaArtist READ mediaArtist NOTIFY mediaChanged)
    Q_PROPERTY(bool isMediaPlaying READ isMediaPlaying NOTIFY mediaChanged)

public:
    explicit SystemHardwareController(QObject *parent = nullptr);

    int volume() const { return m_volume; }
    void setVolume(int vol);

    bool isMuted() const { return m_isMuted; }
    void setIsMuted(bool muted);

    int brightness() const { return m_brightness; }
    void setBrightness(int b);

    bool wifiEnabled() const { return m_wifiEnabled; }
    QString wifiSsid() const { return m_wifiSsid; }

    bool bluetoothEnabled() const { return m_bluetoothEnabled; }

    bool hasBattery() const { return m_hasBattery; }
    int batteryPercentage() const { return m_batteryPercentage; }
    bool isCharging() const { return m_isCharging; }

    bool doNotDisturb() const { return m_doNotDisturb; }
    void setDoNotDisturb(bool dnd);

    bool visible() const { return m_visible; }
    void setVisible(bool v);

    QString mediaTitle() const { return m_mediaTitle; }
    QString mediaArtist() const { return m_mediaArtist; }
    bool isMediaPlaying() const { return m_isMediaPlaying; }

    Q_INVOKABLE void toggleControlCenter();
    Q_INVOKABLE void toggleMute();
    Q_INVOKABLE void toggleWifi();
    Q_INVOKABLE void toggleBluetooth();
    Q_INVOKABLE void mediaPlayPause();
    Q_INVOKABLE void mediaNext();
    Q_INVOKABLE void mediaPrevious();
    Q_INVOKABLE void refreshHardwareStatus();

Q_SIGNALS:
    void volumeChanged();
    void isMutedChanged();
    void brightnessChanged();
    void wifiEnabledChanged();
    void wifiSsidChanged();
    void bluetoothEnabledChanged();
    void batteryChanged();
    void doNotDisturbChanged();
    void visibleChanged();
    void mediaChanged();

    // Señal para mostrar el overlay OSD tipo bezel
    void osdTriggered(const QString &icon, int level, const QString &label);

private:
    void queryPulseAudio();
    void queryBacklight();
    void queryNetworkManager();
    void queryBluez();
    void queryUPower();
    void queryMpris();

    int m_volume = 75;
    bool m_isMuted = false;
    int m_brightness = 80;
    bool m_wifiEnabled = true;
    QString m_wifiSsid = QStringLiteral("Quartz-Net");
    bool m_bluetoothEnabled = true;
    bool m_hasBattery = true;
    int m_batteryPercentage = 100;
    bool m_isCharging = true;
    bool m_doNotDisturb = false;
    bool m_visible = false;

    QString m_mediaTitle = QStringLiteral("Quartz Sound");
    QString m_mediaArtist = QStringLiteral("Desktop Audio");
    bool m_isMediaPlaying = false;

    QTimer *m_pollTimer = nullptr;
};

} // namespace Quartz::Shell
