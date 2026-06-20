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


#include <QUrlQuery>

void FileShareManager::onFileUploadRequest(const QHttpServerRequest &request)
{
    // Retrieve filename from query parameters
    QUrlQuery query(request.url().query());
    QString fileName = query.queryItemValue("filename");

    if (fileName.isEmpty()) {
        qWarning() << "FileShareManager: Received upload request without a filename.";
        return;
    }

    // The body contains the raw file bytes! No Base64 or JSON parsing needed.
    QByteArray fileData = request.body();

    if (fileData.isEmpty()) {
        qWarning() << "FileShareManager: Received empty file upload.";
        return;
    }

    // Get the user's "Downloads" folder
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if(savePath.isEmpty()) {
        savePath = QDir::homePath(); // Fallback to home directory
    }

    QString fullFilePath = savePath + "/" + fileName;

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
