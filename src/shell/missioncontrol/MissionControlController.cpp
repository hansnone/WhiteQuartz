#include "MissionControlController.h"
#include <QDBusMessage>
#include <QDBusReply>

namespace Quartz::Shell {

MissionControlController::MissionControlController(QObject *parent)
    : QObject(parent)
{
    initSpaces();

    // Conectar a señal spatialModeChanged del compositor
    QDBusConnection::sessionBus().connect(
        QStringLiteral("org.quartz.Compositor"),
        QStringLiteral("/org/quartz/Compositor/StageManager"),
        QStringLiteral("org.quartz.Compositor.StageManager"),
        QStringLiteral("spatialModeChanged"),
        this,
        SLOT(toggle())
    );
}

void MissionControlController::initSpaces()
{
    m_spaces.clear();

    // Intentar leer escritorios virtuales de KWin
    QDBusInterface kwinIface(
        QStringLiteral("org.kde.KWin"),
        QStringLiteral("/VirtualDesktopManager"),
        QStringLiteral("org.kde.KWin.VirtualDesktopManager"),
        QDBusConnection::sessionBus()
    );

    if (kwinIface.isValid()) {
        uint count = kwinIface.property("count").toUInt();
        if (count > 0) {
            for (uint i = 0; i < count; ++i) {
                QVariantMap space;
                space[QStringLiteral("id")] = QString::number(i + 1);
                space[QStringLiteral("name")] = QStringLiteral("Escritorio %1").arg(i + 1);
                space[QStringLiteral("active")] = (i == 0);
                m_spaces.append(space);
            }
            Q_EMIT spacesChanged();
            return;
        }
    }

    // Inicialización por defecto (Spaces 1 y 2)
    QVariantMap s1;
    s1[QStringLiteral("id")] = QStringLiteral("1");
    s1[QStringLiteral("name")] = QStringLiteral("Escritorio 1");
    s1[QStringLiteral("active")] = true;
    m_spaces.append(s1);

    QVariantMap s2;
    s2[QStringLiteral("id")] = QStringLiteral("2");
    s2[QStringLiteral("name")] = QStringLiteral("Escritorio 2");
    s2[QStringLiteral("active")] = false;
    m_spaces.append(s2);

    Q_EMIT spacesChanged();
}

void MissionControlController::setActive(bool act)
{
    if (m_active != act) {
        m_active = act;
        Q_EMIT activeChanged(m_active);
    }
}

void MissionControlController::setCurrentSpaceIndex(int idx)
{
    if (idx >= 0 && idx < m_spaces.size() && m_currentSpaceIndex != idx) {
        m_currentSpaceIndex = idx;
        for (int i = 0; i < m_spaces.size(); ++i) {
            QVariantMap map = m_spaces[i].toMap();
            map[QStringLiteral("active")] = (i == m_currentSpaceIndex);
            m_spaces[i] = map;
        }
        Q_EMIT currentSpaceIndexChanged(m_currentSpaceIndex);
        Q_EMIT spacesChanged();
    }
}

void MissionControlController::toggle()
{
    setActive(!m_active);
}

void MissionControlController::addSpace()
{
    int nextId = m_spaces.size() + 1;
    QVariantMap newSpace;
    newSpace[QStringLiteral("id")] = QString::number(nextId);
    newSpace[QStringLiteral("name")] = QStringLiteral("Escritorio %1").arg(nextId);
    newSpace[QStringLiteral("active")] = false;
    m_spaces.append(newSpace);

    // Notificar a KWin si está presente
    QDBusMessage msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.kde.KWin"),
        QStringLiteral("/VirtualDesktopManager"),
        QStringLiteral("org.kde.KWin.VirtualDesktopManager"),
        QStringLiteral("createDesktop")
    );
    msg << (uint)(nextId - 1) << newSpace[QStringLiteral("name")].toString();
    QDBusConnection::sessionBus().send(msg);

    Q_EMIT spacesChanged();
}

void MissionControlController::removeSpace(int index)
{
    if (m_spaces.size() <= 1 || index < 0 || index >= m_spaces.size()) {
        return;
    }

    m_spaces.removeAt(index);
    if (m_currentSpaceIndex >= m_spaces.size()) {
        m_currentSpaceIndex = m_spaces.size() - 1;
    }
    setCurrentSpaceIndex(m_currentSpaceIndex);
    Q_EMIT spacesChanged();
}

void MissionControlController::switchToSpace(int index)
{
    setCurrentSpaceIndex(index);
    setActive(false);
}

void MissionControlController::activateWindow(const QString &/*windowId*/)
{
    setActive(false);
}

void MissionControlController::updateWindows(const QVariantList &winList)
{
    m_windows = winList;
    Q_EMIT windowsChanged();
}

} // namespace Quartz::Shell
