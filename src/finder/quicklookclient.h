#pragma once

#include <QObject>
#include <QString>
#include <QDBusInterface>
#include <QRect>

namespace Quartz::Finder {

class QuickLookClient : public QObject {
    Q_OBJECT

public:
    explicit QuickLookClient(QObject *parent = nullptr);

    void togglePreview(const QString &filePath, const QRect &sourceRect);

private:
    QDBusInterface m_dbusInterface;
};

} // namespace Quartz::Finder
