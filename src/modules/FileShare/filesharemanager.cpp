#include "filesharemanager.h"

// --- Qt Network Includes ---
#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QNetworkInterface> // For getting the IP address

// --- JSON & File Includes ---
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QJsonArray>

// run the HTTP server on port 8080
const quint16 HTTP_PORT = 8080;
// And the WebSocket server on port 8081
const quint16 WEBSOCKET_PORT = 8081;

FileShareManager::FileShareManager(QObject *parent)
    : QObject{parent}
{
    qDebug() << "FileShareManager initialized.";
}


void FileShareManager::onFileUploadRequest(const QHttpServerRequest &request)
{
    // Parse the JSON body
    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "FileShareManager: Received bad file upload request (invalid JSON).";
        // We don't need to send a response; the server already did.
        return;
    }

    QJsonObject obj = doc.object();
    QString fileName = obj["filename"].toString();
    QString base64Data = obj["data"].toString();

    if (fileName.isEmpty() || base64Data.isEmpty()) {
        qWarning() << "FileShareManager: Received bad file upload request (missing fields).";
        return;
    }

    // Get the user's "Downloads" folder
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if(savePath.isEmpty()) {
        savePath = QDir::homePath(); // Fallback to home directory
    }

    QString fullFilePath = savePath + "/" + fileName;

    // Decode the Base64 data
    QByteArray fileData = QByteArray::fromBase64(base64Data.toUtf8());

    // Save the file
    QFile file(fullFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "FileShareManager: Could not open file for writing:" << fullFilePath;
        emit newNotification(QString("Error: Could not save file %1.").arg(fileName));
        return;
    }

    file.write(fileData);
    file.close();

    qDebug() << "File saved to" << fullFilePath;

    // Emit a success notification for the UI
    emit newNotification(QString("File '%1' received!").arg(fileName));
}


QString FileShareManager::getDownloadsPath() const
{
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if(savePath.isEmpty()) {
        savePath = QDir::homePath(); // Fallback
    }
    return savePath;
}


QHttpServerResponse FileShareManager::handleFileListRequest(const QHttpServerRequest &request)
{
    Q_UNUSED(request);

    QDir downloadsDir(getDownloadsPath());
    QStringList fileList = downloadsDir.entryList(QDir::Files); // Get all files

    QJsonArray jsonArray;
    for (const QString &file : fileList) {
        jsonArray.append(file);
    }

    // Send the JSON array as the response
    return QHttpServerResponse(jsonArray);
}

QHttpServerResponse FileShareManager::handleFileDownloadRequest(const QHttpServerRequest &request)
{
    // Get the filename from the path
    // The path will be "/download/filename.txt"
    QString filename = request.url().path().mid(10); // After "/download/"

    if (filename.isEmpty() || filename.contains("..") || filename.contains("/") || filename.contains("\\")) {
        qWarning() << "FileShareManager: Denied unsafe file request:" << filename;
        return QHttpServerResponse(QHttpServerResponse::StatusCode::BadRequest);
    }

    QString fullPath = getDownloadsPath() + "/" + filename;
    QFile file(fullPath);
    if (!file.exists()) {
        qWarning() << "FileShareManager: Requested file not found:" << fullPath;
        return QHttpServerResponse(QHttpServerResponse::StatusCode::NotFound);
    }

    qDebug() << "Sending file:" << fullPath;

    // Directly return the response object created from the file
    return QHttpServerResponse::fromFile(fullPath);
}
