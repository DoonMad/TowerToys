#include "openurlaction.h"
#include <QDesktopServices>
#include <QUrl>

OpenURLAction::OpenURLAction(const QString& url) : url(url) {}

void OpenURLAction::execute() {
    QDesktopServices::openUrl(QUrl(url));
}

QString OpenURLAction::description() const {
    return "Open URL: " + url;
}

QJsonObject OpenURLAction::toJson() const
{
    QJsonObject obj;
    obj["type"] = "OpenURLAction";
    obj["url"] = url;
    return obj;
}
