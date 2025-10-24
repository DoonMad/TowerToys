#include "openvscodefolderaction.h"
#include <QProcess> // Needed to run external commands
#include <QUrl>     // Needed to format the path as a URI
#include <QDebug>   // For error messages

OpenVSCodeFolderAction::OpenVSCodeFolderAction(const QString& folderPath)
    : folderPath(folderPath) {}

void OpenVSCodeFolderAction::execute()
{
    if (folderPath.isEmpty()) {
        qWarning() << "OpenVSCodeFolderAction: Folder path is empty.";
        return;
    }

    // Format the path as a file URI (e.g., file:///C:/Users/...)
    // QUrl::fromLocalFile handles spaces and platform differences correctly.
    QString folderUri = QUrl::fromLocalFile(folderPath).toString();

    // Arguments for the 'code' command
    QStringList arguments;
    arguments << "--folder-uri" << folderUri;

    // --- TRY THIS ---
    bool success = QProcess::startDetached("code.cmd", arguments);
    // ---

    if (!success) {
        qWarning() << "OpenVSCodeFolderAction: Failed to start 'code.cmd'. Is VS Code installed and in PATH?";
        // Fallback to just 'code' ---
        qWarning() << "OpenVSCodeFolderAction: Trying 'code'...";
        success = QProcess::startDetached("code", arguments);
        if(!success) {
            qWarning() << "OpenVSCodeFolderAction: Failed to start 'code' as well.";
        }
    } else {
        qDebug() << "OpenVSCodeFolderAction: Attempted to open folder:" << folderPath;
    }
}

QString OpenVSCodeFolderAction::description() const
{
    return QString("Open Folder: %1 in VS Code").arg(folderPath);
}

QJsonObject OpenVSCodeFolderAction::toJson() const
{
    QJsonObject obj;
    obj["type"] = "OpenVSCodeFolderAction";
    obj["path"] = folderPath;
    return obj;
}
