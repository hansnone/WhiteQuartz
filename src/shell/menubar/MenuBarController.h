#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>

namespace Quartz::Shell {

class MenuBarController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString formattedTime READ formattedTime NOTIFY timeChanged)
    Q_PROPERTY(QString activeAppTitle READ activeAppTitle NOTIFY activeAppTitleChanged)
    Q_PROPERTY(bool isAltPressed READ isAltPressed NOTIFY altPressedChanged)

public:
    explicit MenuBarController(QObject *parent = nullptr);

    QString formattedTime() const { return m_formattedTime; }
    QString activeAppTitle() const { return m_activeAppTitle; }
    bool isAltPressed() const { return m_isAltPressed; }

public Q_SLOTS:
    void setActiveAppTitle(const QString &title);
    void triggerAppleMenuAction(const QString &actionId);
    void setAltPressed(bool pressed);
    void toggleNotificationCenter();

Q_SIGNALS:
    void timeChanged();
    void activeAppTitleChanged();
    void altPressedChanged(bool pressed);
    void notificationCenterToggled();

private Q_SLOTS:
    void updateClock();

private:
    QString m_formattedTime;
    QString m_activeAppTitle = QStringLiteral("Finder");
    bool m_isAltPressed = false;
    QTimer m_clockTimer;
};

} // namespace Quartz::Shell
