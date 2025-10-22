#include "openappaction.h"
#include <QProcess>

void OpenAppAction::execute() {
    QProcess::startDetached(appPath);
}

QJsonObject OpenAppAction::toJson() const
{
    QJsonObject obj;
    obj["type"] = "OpenAppAction";
    obj["path"] = appPath;
    return obj;
}
