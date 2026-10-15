#include "quicklookclient.h"
#include <QDBusMessage>

namespace Quartz::Finder {

QuickLookClient::QuickLookClient(QObject *parent)
    : QObject(parent)
    , m_dbusInterface(QStringLiteral("org.quartz.QuickLook"),
                      QStringLiteral("/org/quartz/QuickLook"),
                      QStringLiteral("org.quartz.QuickLook"),
                      QDBusConnection::sessionBus(),
                      parent)
{
}

void QuickLookClient::togglePreview(const QString &filePath, const QRect &sourceRect)
{
    QString uri = filePath.startsWith(QStringLiteral("file://"))
        ? filePath
        : QStringLiteral("file://") + filePath;

    // Llamada asíncrona no bloqueante
    m_dbusInterface.asyncCall(QStringLiteral("TogglePreview"),
                             uri,
                             QVariant::fromValue(QList<int>{ sourceRect.x(), sourceRect.y(), sourceRect.width(), sourceRect.height() }));
}

} // namespace Quartz::Finder
