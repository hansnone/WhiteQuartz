#pragma once

#include <QObject>
#include <QTimer>
#include <QCursor>
#include <QGuiApplication>
#include <QScreen>

namespace Quartz::Shell {

class HotCornersController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit HotCornersController(QObject *parent = nullptr);

    bool enabled() const { return m_enabled; }
    void setEnabled(bool e);

Q_SIGNALS:
    void enabledChanged(bool e);
    void cornerTriggered(int cornerIndex); // 0=TopLeft, 1=TopRight, 2=BottomLeft, 3=BottomRight

private Q_SLOTS:
    void checkCursorPosition();

private:
    bool m_enabled = true;
    QTimer *m_pollTimer = nullptr;
    int m_activeCorner = -1;
    int m_hoverDuration = 0;
};

} // namespace Quartz::Shell
