#ifndef LOCALSHARESERVER_H
#define LOCALSHARESERVER_H

#include <QObject>
#include <QHttpServer>
#include <QList>
#include <QHttpServerResponder>

class QWebSocketServer;
class QWebSocket;
class QTcpServer;
class FileShareManager;
struct QHttpServerRequest;

class LocalShareServer : public QObject
{
    Q_OBJECT
public:
    explicit LocalShareServer(QObject *parent = nullptr);
    ~LocalShareServer();

    static QString getLocalIpAddress();
    void setFileShareManager(FileShareManager* manager);

public slots:
    void startServer();
    void stopServer();

    // --- Data Pushing ---
    /**
     * @brief Pushes a string to all connected web clients.
     * (Used by ClipboardSyncManager)
     */
    void pushTextToClients(const QString &text);

signals:
    void serverStatusChanged(bool running, const QString &serverUrl);

    // --- Data Receiving ---
    /**
     * @brief Emitted when a client uploads a file.
     * (Connected to by FileShareManager)
     */
    void fileUploadRequest(const QHttpServerRequest &request);

    /**
     * @brief Emitted when a client requests the current clipboard.
     * (Connected to by ClipboardSyncManager)
     */
    void clipboardDataRequest(const QHttpServerRequest &request);

    // for downloading files from the server
    // void fileListRequest(const QHttpServerRequest &request);
    // void fileDownloadRequest(const QHttpServerRequest &request);

private slots:
    // --- WebSocket Logic ---
    void onNewWebSocketConnection();
    void onWebSocketDisconnected();

private:
    QHttpServerResponse handleMainPage(const QHttpServerRequest &request);
    QString getEmbeddedWebpage() const;

    QHttpServer httpServer;
    QWebSocketServer* webSocketServer;
    QTcpServer* tcpServer;
    FileShareManager* fileShareManager = nullptr;

    QList<QWebSocket*> clients;
};

#endif // LOCALSHARESERVER_H
