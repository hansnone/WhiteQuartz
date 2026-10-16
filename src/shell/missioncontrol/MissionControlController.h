#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QDBusConnection>
#include <QDBusInterface>

namespace Quartz::Shell {

class MissionControlController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QVariantList spaces READ spaces NOTIFY spacesChanged)
    Q_PROPERTY(int currentSpaceIndex READ currentSpaceIndex WRITE setCurrentSpaceIndex NOTIFY currentSpaceIndexChanged)
    Q_PROPERTY(QVariantList windows READ windows NOTIFY windowsChanged)

public:
    explicit MissionControlController(QObject *parent = nullptr);

    bool active() const { return m_active; }
    void setActive(bool act);

    QVariantList spaces() const { return m_spaces; }
    int currentSpaceIndex() const { return m_currentSpaceIndex; }
    void setCurrentSpaceIndex(int idx);

    QVariantList windows() const { return m_windows; }

public Q_SLOTS:
    void toggle();
    void addSpace();
    void removeSpace(int index);
    void switchToSpace(int index);
    void activateWindow(const QString &windowId);
    void updateWindows(const QVariantList &winList);

Q_SIGNALS:
    void activeChanged(bool active);
    void spacesChanged();
    void currentSpaceIndexChanged(int index);
    void windowsChanged();

private:
    bool m_active = false;
    int m_currentSpaceIndex = 0;
    QVariantList m_spaces;
    QVariantList m_windows;
    void initSpaces();
};

} // namespace Quartz::Shell
