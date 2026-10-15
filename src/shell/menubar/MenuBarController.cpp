#include "MenuBarController.h"
#include <QProcess>

namespace Quartz::Shell {

MenuBarController::MenuBarController(QObject *parent)
    : QObject(parent)
{
    updateClock();
    connect(&m_clockTimer, &QTimer::timeout, this, &MenuBarController::updateClock);
    m_clockTimer.start(1000);
}

void MenuBarController::updateClock()
{
    QString now = QDateTime::currentDateTime().toString(QStringLiteral("ddd d MMM  HH:mm"));
    if (m_formattedTime != now) {
        m_formattedTime = now;
        Q_EMIT timeChanged();
    }
}

void MenuBarController::setActiveAppTitle(const QString &title)
{
    if (m_activeAppTitle != title) {
        m_activeAppTitle = title;
        Q_EMIT activeAppTitleChanged();
    }
}

void MenuBarController::setAltPressed(bool pressed)
{
    if (m_isAltPressed != pressed) {
        m_isAltPressed = pressed;
        Q_EMIT altPressedChanged(m_isAltPressed);
    }
}

void MenuBarController::toggleNotificationCenter()
{
    Q_EMIT notificationCenterToggled();
}

void MenuBarController::triggerAppleMenuAction(const QString &actionId)
{
    if (actionId == QStringLiteral("about")) {
        QProcess::startDetached(QStringLiteral("quartz-settings"), { QStringLiteral("--pane"), QStringLiteral("general") });
    } else if (actionId == QStringLiteral("settings")) {
        QProcess::startDetached(QStringLiteral("quartz-settings"), {});
    } else if (actionId == QStringLiteral("sleep")) {
        QProcess::startDetached(QStringLiteral("systemctl"), { QStringLiteral("suspend") });
    } else if (actionId == QStringLiteral("reboot")) {
        QProcess::startDetached(QStringLiteral("systemctl"), { QStringLiteral("reboot") });
    } else if (actionId == QStringLiteral("shutdown")) {
        QProcess::startDetached(QStringLiteral("systemctl"), { QStringLiteral("poweroff") });
    }
}

} // namespace Quartz::Shell
