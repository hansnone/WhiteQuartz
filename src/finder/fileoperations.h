#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QUrl>
#include <QUndoStack>
#include <QUndoCommand>
#include <functional>

namespace Quartz::Finder {

class FileOperations : public QObject {
    Q_OBJECT

public:
    explicit FileOperations(QObject *parent = nullptr);
    ~FileOperations() override = default;

    // Generación de nombres y validación
    static QString generateUniqueFolderName(const QString &parentDir);
    static QString generateDuplicatePath(const QString &sourcePath);
    static bool isValidFileName(const QString &name);

    // Atributos extendidos (user.xdg.tags)
    static bool setFileTag(const QString &filePath, const QString &tagName);
    static QString getFileTag(const QString &filePath);
    static QStringList getAllTags(const QString &filePath);
    static QColor colorForTag(const QString &tagName);
    static QStringList standardTagNames();

    // Portapapeles estándar (text/uri-list + application/x-kde-cutselection)
    static void copyToClipboard(const QStringList &filePaths, bool isCut);
    static bool isCutOperationInClipboard();
    static QStringList clipboardFilePaths();

    // Operaciones síncronas de bajo nivel
    static bool createFolder(const QString &parentDir, const QString &name = QString(), QString *createdPath = nullptr);
    static bool duplicateFileOrDirectory(const QString &sourcePath, QString *duplicatedPath = nullptr);
    static bool renameFileOrDirectory(const QString &sourcePath, const QString &newName, QString *newPath = nullptr);
    static bool moveToTrash(const QString &filePath);
    static bool restoreFromTrash(const QString &trashInfoPath);
    static bool copyRecursively(const QString &src, const QString &dst,
                                std::function<void(qint64 done, qint64 total)> progressCallback = nullptr);
    static bool moveRecursively(const QString &src, const QString &dst);

    // Operaciones asíncronas para la interfaz
    void copyFilesAsync(const QStringList &sources, const QString &destinationDir);
    void moveFilesAsync(const QStringList &sources, const QString &destinationDir);

    // Gestión de Deshacer / Rehacer
    QUndoStack *undoStack() { return &m_undoStack; }

Q_SIGNALS:
    void progressChanged(qint64 bytesDone, qint64 bytesTotal);
    void operationFinished(bool success, const QString &errorMessage);

private:
    QUndoStack m_undoStack;
};

// Comandos de QUndoStack
class RenameCommand : public QUndoCommand {
public:
    RenameCommand(const QString &oldPath, const QString &newPath, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    QString m_oldPath;
    QString m_newPath;
};

class CreateFolderCommand : public QUndoCommand {
public:
    explicit CreateFolderCommand(const QString &folderPath, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    QString m_folderPath;
};

} // namespace Quartz::Finder
