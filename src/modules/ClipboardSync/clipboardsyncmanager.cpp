#include "clipboardsyncmanager.h"
#include <QHttpServerRequest>
#include <QHttpServerResponse> // Needed for sending the response
#include <QJsonObject>
#include <QDebug>

ClipboardSyncManager::ClipboardSyncManager(QObject *parent)
    : QObject{parent}
{
    qDebug() << "ClipboardSyncManager initialized.";
}

void ClipboardSyncManager::updateClipboardText(const QString &text)
{
    currentClipboardText = text;
    // We don't push here; AppController tells LocalShareServer to push
}

void ClipboardSyncManager::handleClipboardDataRequest(const QHttpServerRequest &request, QHttpServerResponder &responder)
{
    Q_UNUSED(request); // We don't need the request details for this

    qDebug() << "ClipboardSyncManager: Responding to clipboard request.";

    // Create a simple JSON object: { "clipboard": "the text" }
    QJsonObject obj;
    obj["clipboard"] = currentClipboardText;

    responder.sendResponse(QHttpServerResponse(obj));
}
