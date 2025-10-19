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
