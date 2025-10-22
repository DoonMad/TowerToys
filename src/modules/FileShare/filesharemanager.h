#ifndef FILESHAREMANAGER_H
#define FILESHAREMANAGER_H

#include <QObject>
#include <QHttpServer>
#include <QList>
#include <QHttpServerResponder>

class QWebSocketServer;
class QWebSocket;
struct QHttpServerRequest;

class FileShareManager : public QObject
{
    Q_OBJECT
public:
    explicit FileShareManager(QObject *parent = nullptr);

    // @brief Gets the local (Wi-Fi) IP address of this PC. return a string (e.g., "192.168.1.10") or "Not Found".
    static QString getLocalIpAddress();

public slots:
    /**
     * @brief This slot connects to LocalShareServer::fileUploadRequest.
     * It does the work of parsing the request and saving the file.
     */
    void onFileUploadRequest(const QHttpServerRequest &request);
    QHttpServerResponse handleFileListRequest(const QHttpServerRequest &request);
    QHttpServerResponse handleFileDownloadRequest(const QHttpServerRequest &request);

signals:
    /**
     * @brief Emits a message for the UI to show a notification.
     * @param message The message (e.g., "File 'report.pdf' received!").
     */
    void newNotification(const QString &message);

private:
    QString getDownloadsPath() const;

};

#endif // FILESHAREMANAGER_H
