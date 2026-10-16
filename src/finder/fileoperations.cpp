#include "fileoperations.h"
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QFile>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QMimeDatabase>
#include <QtConcurrent/QtConcurrent>
#include <sys/xattr.h>
#include <errno.h>

namespace Quartz::Finder {

FileOperations::FileOperations(QObject *parent)
    : QObject(parent)
{
}

QString FileOperations::generateUniqueFolderName(const QString &parentDir)
{
    QString baseName = QStringLiteral("carpeta sin título");
    QString targetPath = parentDir + QStringLiteral("/") + baseName;
    if (!QFile::exists(targetPath)) {
        return baseName;
    }

    int counter = 2;
    while (true) {
        QString candidate = QStringLiteral("carpeta sin título %1").arg(counter);
        if (!QFile::exists(parentDir + QStringLiteral("/") + candidate)) {
            return candidate;
        }
        ++counter;
    }
}

QString FileOperations::generateDuplicatePath(const QString &sourcePath)
{
    QFileInfo fi(sourcePath);
    QString dir = fi.absolutePath();
    QString fileName = fi.fileName();

    if (fi.isDir()) {
        QString baseName = fileName;
        QString candidate = dir + QStringLiteral("/") + baseName + QStringLiteral(" copia");
        if (!QFile::exists(candidate)) {
            return candidate;
        }

        int counter = 2;
        while (true) {
            candidate = dir + QStringLiteral("/") + baseName + QStringLiteral(" copia %1").arg(counter);
            if (!QFile::exists(candidate)) {
                return candidate;
            }
            ++counter;
        }
    } else {
        // Manejar extensiones compuestas (.tar.gz, .tar.bz2, etc.)
        QString lowerName = fileName.toLower();
        QString baseName;
        QString ext;

        if (lowerName.endsWith(QStringLiteral(".tar.gz"))) {
            baseName = fileName.left(fileName.length() - 7);
            ext = QStringLiteral(".tar.gz");
        } else if (lowerName.endsWith(QStringLiteral(".tar.bz2"))) {
            baseName = fileName.left(fileName.length() - 8);
            ext = QStringLiteral(".tar.bz2");
        } else if (lowerName.endsWith(QStringLiteral(".tar.xz"))) {
            baseName = fileName.left(fileName.length() - 7);
            ext = QStringLiteral(".tar.xz");
        } else {
            ext = fi.suffix().isEmpty() ? QString() : (QStringLiteral(".") + fi.suffix());
            baseName = fi.completeBaseName();
        }

        QString candidate = dir + QStringLiteral("/") + baseName + QStringLiteral(" copia") + ext;
        if (!QFile::exists(candidate)) {
            return candidate;
        }

        int counter = 2;
        while (true) {
            candidate = dir + QStringLiteral("/") + baseName + QStringLiteral(" copia %1").arg(counter) + ext;
            if (!QFile::exists(candidate)) {
                return candidate;
            }
            ++counter;
        }
    }
}

bool FileOperations::isValidFileName(const QString &name)
{
    QString trimmed = name.trimmed();
    if (trimmed.isEmpty()) {
        return false;
    }
    if (trimmed == QStringLiteral(".") || trimmed == QStringLiteral("..")) {
        return false;
    }
    if (trimmed.contains(QLatin1Char('/')) || trimmed.contains(QLatin1Char('\0'))) {
        return false;
    }
    return true;
}

QStringList FileOperations::standardTagNames()
{
    return {
        QStringLiteral("Rojo"),
        QStringLiteral("Naranja"),
        QStringLiteral("Amarillo"),
        QStringLiteral("Verde"),
        QStringLiteral("Azul"),
        QStringLiteral("Morado"),
        QStringLiteral("Gris")
    };
}

QColor FileOperations::colorForTag(const QString &tagName)
{
    if (tagName == QStringLiteral("Rojo")) return QColor(QStringLiteral("#FF3B30"));
    if (tagName == QStringLiteral("Naranja")) return QColor(QStringLiteral("#FF9500"));
    if (tagName == QStringLiteral("Amarillo")) return QColor(QStringLiteral("#FFCC00"));
    if (tagName == QStringLiteral("Verde")) return QColor(QStringLiteral("#34C759"));
    if (tagName == QStringLiteral("Azul")) return QColor(QStringLiteral("#007AFF"));
    if (tagName == QStringLiteral("Morado")) return QColor(QStringLiteral("#AF52DE"));
    if (tagName == QStringLiteral("Gris")) return QColor(QStringLiteral("#8E8E93"));
    return QColor();
}

bool FileOperations::setFileTag(const QString &filePath, const QString &tagName)
{
    QByteArray pathBa = QFile::encodeName(filePath);
    QStringList existingTags = getAllTags(filePath);
    QStringList colorTags = standardTagNames();

    // Eliminar etiquetas de color previas
    QStringList updatedTags;
    for (const QString &t : existingTags) {
        if (!colorTags.contains(t, Qt::CaseInsensitive)) {
            updatedTags.append(t);
        }
    }

    // Agregar nueva etiqueta si no está vacía
    if (!tagName.isEmpty()) {
        updatedTags.append(tagName);
    }

    QByteArray value = updatedTags.join(QLatin1Char(',')).toUtf8();
    int res = setxattr(pathBa.constData(), "user.xdg.tags", value.constData(), value.size(), 0);
    return (res == 0);
}

QString FileOperations::getFileTag(const QString &filePath)
{
    QStringList tags = getAllTags(filePath);
    QStringList colorTags = standardTagNames();
    for (const QString &t : tags) {
        for (const QString &c : colorTags) {
            if (t.compare(c, Qt::CaseInsensitive) == 0) {
                return c;
            }
        }
    }
    return QString();
}

QStringList FileOperations::getAllTags(const QString &filePath)
{
    QByteArray pathBa = QFile::encodeName(filePath);
    char buffer[1024];
    ssize_t size = getxattr(pathBa.constData(), "user.xdg.tags", buffer, sizeof(buffer) - 1);
    if (size <= 0) {
        return {};
    }
    buffer[size] = '\0';
    QString raw = QString::fromUtf8(buffer);
    return raw.split(QLatin1Char(','), Qt::SkipEmptyParts);
}

void FileOperations::copyToClipboard(const QStringList &filePaths, bool isCut)
{
    if (filePaths.isEmpty()) {
        return;
    }

    auto *mimeData = new QMimeData();
    QList<QUrl> urls;
    for (const QString &p : filePaths) {
        urls.append(QUrl::fromLocalFile(p));
    }
    mimeData->setUrls(urls);
    mimeData->setText(filePaths.join(QLatin1Char('\n')));

    if (isCut) {
        mimeData->setData(QStringLiteral("application/x-kde-cutselection"), QByteArrayLiteral("1"));
    }

    QApplication::clipboard()->setMimeData(mimeData);
}

bool FileOperations::isCutOperationInClipboard()
{
    const QMimeData *mime = QApplication::clipboard()->mimeData();
    if (!mime) {
        return false;
    }
    return mime->data(QStringLiteral("application/x-kde-cutselection")) == QByteArrayLiteral("1");
}

QStringList FileOperations::clipboardFilePaths()
{
    const QMimeData *mime = QApplication::clipboard()->mimeData();
    if (!mime) {
        return {};
    }

    QStringList result;
    for (const QUrl &url : mime->urls()) {
        if (url.isLocalFile()) {
            result.append(url.toLocalFile());
        }
    }
    return result;
}

bool FileOperations::createFolder(const QString &parentDir, const QString &name, QString *createdPath)
{
    QString folderName = name.isEmpty() ? generateUniqueFolderName(parentDir) : name;
    if (!isValidFileName(folderName)) {
        return false;
    }

    QString fullPath = parentDir + QStringLiteral("/") + folderName;
    QDir dir(parentDir);
    if (dir.mkdir(folderName)) {
        if (createdPath) {
            *createdPath = fullPath;
        }
        return true;
    }
    return false;
}

bool FileOperations::duplicateFileOrDirectory(const QString &sourcePath, QString *duplicatedPath)
{
    QString targetPath = generateDuplicatePath(sourcePath);
    bool ok = copyRecursively(sourcePath, targetPath);
    if (ok && duplicatedPath) {
        *duplicatedPath = targetPath;
    }
    return ok;
}

bool FileOperations::renameFileOrDirectory(const QString &sourcePath, const QString &newName, QString *newPath)
{
    if (!isValidFileName(newName)) {
        return false;
    }

    QFileInfo fi(sourcePath);
    QString target = fi.absolutePath() + QStringLiteral("/") + newName;
    if (QFile::exists(target)) {
        return false;
    }

    if (QFile::rename(sourcePath, target)) {
        if (newPath) {
            *newPath = target;
        }
        return true;
    }
    return false;
}

bool FileOperations::moveToTrash(const QString &filePath)
{
    return QFile::moveToTrash(filePath);
}

bool FileOperations::restoreFromTrash(const QString &trashInfoPath)
{
    QFile file(trashInfoPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QString originalPath;
    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine()).trimmed();
        if (line.startsWith(QLatin1String("Path="))) {
            originalPath = QUrl::fromPercentEncoding(line.mid(5).toUtf8());
            break;
        }
    }
    file.close();

    if (originalPath.isEmpty()) {
        return false;
    }

    QFileInfo infoFi(trashInfoPath);
    QString baseName = infoFi.completeBaseName();
    QString trashDir = infoFi.absolutePath();
    // Reemplazar /info por /files
    if (trashDir.endsWith(QLatin1String("/info"))) {
        trashDir = trashDir.left(trashDir.length() - 5) + QStringLiteral("/files");
    }

    QString trashFilePath = trashDir + QStringLiteral("/") + baseName;
    if (!QFile::exists(trashFilePath)) {
        return false;
    }

    QDir().mkpath(QFileInfo(originalPath).absolutePath());
    if (QFile::rename(trashFilePath, originalPath)) {
        QFile::remove(trashInfoPath);
        return true;
    }
    return false;
}

bool FileOperations::copyRecursively(const QString &src, const QString &dst,
                                     std::function<void(qint64 done, qint64 total)> progressCallback)
{
    QString cleanSrc = QDir::cleanPath(src);
    QString cleanDst = QDir::cleanPath(dst);

    // Protección estricta contra recursión infinita
    if (cleanDst.startsWith(cleanSrc + QStringLiteral("/"))) {
        return false;
    }

    QFileInfo srcInfo(cleanSrc);
    if (!srcInfo.exists()) {
        return false;
    }

    if (srcInfo.isSymLink()) {
        QString target = srcInfo.symLinkTarget();
        return QFile::link(target, cleanDst);
    }

    if (srcInfo.isFile()) {
        QString tmpDst = cleanDst + QStringLiteral(".tmp.") + QString::number(QCoreApplication::applicationPid());
        if (QFile::copy(cleanSrc, tmpDst)) {
            QFile::setPermissions(tmpDst, srcInfo.permissions());
            QFile device(tmpDst);
            if (device.open(QIODevice::WriteOnly | QIODevice::Append)) {
                device.setFileTime(srcInfo.fileTime(QFileDevice::FileModificationTime), QFileDevice::FileModificationTime);
                device.close();
            }

            // Copiar xattrs (user.xdg.tags)
            QString tag = getFileTag(cleanSrc);
            if (!tag.isEmpty()) {
                setFileTag(tmpDst, tag);
            }

            if (QFile::rename(tmpDst, cleanDst)) {
                if (progressCallback) {
                    progressCallback(srcInfo.size(), srcInfo.size());
                }
                return true;
            }
            QFile::remove(tmpDst);
        }
        return false;
    }

    if (srcInfo.isDir()) {
        if (!QDir().mkpath(cleanDst)) {
            return false;
        }

        QDir dir(cleanSrc);
        QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
        qint64 totalSize = 0;
        for (const QFileInfo &e : entries) {
            totalSize += e.size();
        }

        qint64 processedSize = 0;
        for (const QFileInfo &entry : entries) {
            QString subSrc = entry.absoluteFilePath();
            QString subDst = cleanDst + QStringLiteral("/") + entry.fileName();
            if (!copyRecursively(subSrc, subDst)) {
                return false;
            }
            processedSize += entry.size();
            if (progressCallback) {
                progressCallback(processedSize, totalSize);
            }
        }
        return true;
    }

    return false;
}

bool FileOperations::moveRecursively(const QString &src, const QString &dst)
{
    if (QFile::rename(src, dst)) {
        return true;
    }
    // Si falla por estar en diferente volumen, copiar y luego borrar origen
    if (copyRecursively(src, dst)) {
        QFileInfo fi(src);
        if (fi.isDir()) {
            QDir(src).removeRecursively();
        } else {
            QFile::remove(src);
        }
        return true;
    }
    return false;
}

void FileOperations::copyFilesAsync(const QStringList &sources, const QString &destinationDir)
{
    (void)QtConcurrent::run([this, sources, destinationDir]() {
        bool overallOk = true;
        QString errorMsg;

        for (const QString &src : sources) {
            QFileInfo fi(src);
            QString dst = destinationDir + QStringLiteral("/") + fi.fileName();
            if (QFile::exists(dst)) {
                dst = generateDuplicatePath(dst);
            }

            if (!copyRecursively(src, dst, [this](qint64 done, qint64 total) {
                Q_EMIT progressChanged(done, total);
            })) {
                overallOk = false;
                errorMsg = QStringLiteral("Error al copiar: %1").arg(src);
                break;
            }
        }

        Q_EMIT operationFinished(overallOk, errorMsg);
    });
}

void FileOperations::moveFilesAsync(const QStringList &sources, const QString &destinationDir)
{
    (void)QtConcurrent::run([this, sources, destinationDir]() {
        bool overallOk = true;
        QString errorMsg;

        for (const QString &src : sources) {
            QFileInfo fi(src);
            QString dst = destinationDir + QStringLiteral("/") + fi.fileName();
            if (QFile::exists(dst)) {
                dst = generateDuplicatePath(dst);
            }

            if (!moveRecursively(src, dst)) {
                overallOk = false;
                errorMsg = QStringLiteral("Error al mover: %1").arg(src);
                break;
            }
        }

        Q_EMIT operationFinished(overallOk, errorMsg);
    });
}

// Implementación de QUndoCommand
RenameCommand::RenameCommand(const QString &oldPath, const QString &newPath, QUndoCommand *parent)
    : QUndoCommand(parent), m_oldPath(oldPath), m_newPath(newPath)
{
    setText(QStringLiteral("Renombrar %1").arg(QFileInfo(oldPath).fileName()));
}

void RenameCommand::undo()
{
    QFile::rename(m_newPath, m_oldPath);
}

void RenameCommand::redo()
{
    QFile::rename(m_oldPath, m_newPath);
}

CreateFolderCommand::CreateFolderCommand(const QString &folderPath, QUndoCommand *parent)
    : QUndoCommand(parent), m_folderPath(folderPath)
{
    setText(QStringLiteral("Crear carpeta %1").arg(QFileInfo(folderPath).fileName()));
}

void CreateFolderCommand::undo()
{
    QDir(m_folderPath).removeRecursively();
}

void CreateFolderCommand::redo()
{
    QDir().mkpath(m_folderPath);
}

} // namespace Quartz::Finder
