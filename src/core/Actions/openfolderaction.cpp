#include "openfolderaction.h"
#include <QDesktopServices>
#include <QUrl>

OpenFolderAction::OpenFolderAction(const QString& path) : folderPath(path) {}

void OpenFolderAction::execute()
{
    if (!folderPath.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
    }
}

QString OpenFolderAction::description() const
{
    return "Open Folder: " + folderPath;
}

QJsonObject OpenFolderAction::toJson() const
{
    QJsonObject obj;
    obj["type"] = "OpenFolderAction";
    obj["path"] = folderPath;
    return obj;
}
