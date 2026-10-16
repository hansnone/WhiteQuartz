#include "HotCornersController.h"
#include <QPoint>

namespace Quartz::Shell {

HotCornersController::HotCornersController(QObject *parent)
    : QObject(parent)
{
    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(50); // Muestreo de 50ms para respuesta fluida sin carga de CPU
    connect(m_pollTimer, &QTimer::timeout, this, &HotCornersController::checkCursorPosition);
    m_pollTimer->start();
}

void HotCornersController::setEnabled(bool e)
{
    if (m_enabled != e) {
        m_enabled = e;
        Q_EMIT enabledChanged(m_enabled);
        if (m_enabled) {
            m_pollTimer->start();
        } else {
            m_pollTimer->stop();
        }
    }
}

void HotCornersController::checkCursorPosition()
{
    if (!m_enabled) return;

    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    QRect geom = screen->geometry();
    QPoint pos = QCursor::pos();

    const int threshold = 5; // Margen de detección en px

    int corner = -1;
    if (pos.x() <= geom.left() + threshold && pos.y() <= geom.top() + threshold) {
        corner = 0; // TopLeft
    } else if (pos.x() >= geom.right() - threshold && pos.y() <= geom.top() + threshold) {
        corner = 1; // TopRight
    } else if (pos.x() <= geom.left() + threshold && pos.y() >= geom.bottom() - threshold) {
        corner = 2; // BottomLeft
    } else if (pos.x() >= geom.right() - threshold && pos.y() >= geom.bottom() - threshold) {
        corner = 3; // BottomRight
    }

    if (corner != -1) {
        if (m_activeCorner == corner) {
            m_hoverDuration += 50;
            // Disparar tras 200ms de permanencia (evita disparos accidentales)
            if (m_hoverDuration == 200) {
                Q_EMIT cornerTriggered(corner);
            }
        } else {
            m_activeCorner = corner;
            m_hoverDuration = 50;
        }
    } else {
        m_activeCorner = -1;
        m_hoverDuration = 0;
    }
}

} // namespace Quartz::Shell
