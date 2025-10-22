#include "localshareserver.h"
#include <QWebSocketServer>
#include <QWebSocket>
#include <QNetworkInterface>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QTcpServer>
#include <FileShare/filesharemanager.h>

// HTTP server on port 8080
const quint16 HTTP_PORT = 8080;
// And the WebSocket server on port 8081
const quint16 WEBSOCKET_PORT = 8081;

LocalShareServer::LocalShareServer(QObject *parent)
    : QObject{parent}
    , webSocketServer(new QWebSocketServer(QStringLiteral("LocalShareServer"), QWebSocketServer::NonSecureMode, this))
    , tcpServer(new QTcpServer(this))
{
    httpServer.route("/", QHttpServerRequest::Method::Get, [this](const QHttpServerRequest &req) {
       return handleMainPage(req);
    });

    // Route "/upload"
    // It just emits a signal for another manager to deal with.
    httpServer.route("/upload", QHttpServerRequest::Method::Post, [this](const QHttpServerRequest &request) {
       emit fileUploadRequest(request);
       // We MUST return a response. We'll send a default "OK"
       // The FileShareManager can send a more specific error later if needed.
       return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
    });

    // Route "/clipboard"
    // Same as above, just emit a signal for the ClipboardManager
    httpServer.route("/clipboard", QHttpServerRequest::Method::Get, [this](const QHttpServerRequest &request) {
       emit clipboardDataRequest(request);
       // This MUST be handled by the ClipboardManager, which will send
       // the real response. We return an "in-progress" response.
       return QHttpServerResponse(QHttpServerResponse::StatusCode::Continue);
    });

    httpServer.route("/files", QHttpServerRequest::Method::Get,
                     [this](const QHttpServerRequest &req) {
                         // Directly call the manager (if it exists)
                         if (fileShareManager) {
                             return fileShareManager->handleFileListRequest(req);
                         }
                         // Fallback if manager isn't set
                         return QHttpServerResponse(QHttpServerResponse::StatusCode::InternalServerError);
                     });

    // We use a regex ".*" to capture everything after "/download/"
    httpServer.route(QStringLiteral("/download/.*"), QHttpServerRequest::Method::Get,
                     [this](const QHttpServerRequest &req) {
                         // Directly call the manager (if it exists)
                         if (fileShareManager) {
                             return fileShareManager->handleFileDownloadRequest(req);
                         }
                         // Fallback if manager isn't set
                         return QHttpServerResponse(QHttpServerResponse::StatusCode::InternalServerError);
                     });


    connect(webSocketServer, &QWebSocketServer::newConnection, this, &LocalShareServer::onNewWebSocketConnection);

    qDebug() << "LocalShareServer initialized.";
}

LocalShareServer::~LocalShareServer()
{
    stopServer();
}

void LocalShareServer::setFileShareManager(FileShareManager* manager) {
    fileShareManager = manager;
}

void LocalShareServer::startServer()
{
    bool wsStarted = webSocketServer->listen(QHostAddress::Any, WEBSOCKET_PORT);
    bool httpListened = tcpServer->listen(QHostAddress::Any, HTTP_PORT);
    bool httpBound = false;
    if (httpListened) {
        httpBound = httpServer.bind(tcpServer);
    }

    if (wsStarted && httpBound) {
        QString ip = getLocalIpAddress();
        if(ip == "Not Found") {
            emit serverStatusChanged(false, "Error: No network connection.");
        } else {
            QString url = QString("http://%1:%2").arg(ip).arg(HTTP_PORT);
            emit serverStatusChanged(true, url);
            qDebug() << "LocalShareServer started at" << url;
        }
    } else {
        stopServer();
        emit serverStatusChanged(false, "Error: Could not start server. Port busy?");
        qWarning() << "LocalShareServer failed to start.";
    }
}

void LocalShareServer::stopServer()
{
    tcpServer->close();
    webSocketServer->close();
    qDeleteAll(clients);
    clients.clear();
    emit serverStatusChanged(false, "");
    qDebug() << "LocalShareServer stopped.";
}

void LocalShareServer::pushTextToClients(const QString &text)
{
    for (QWebSocket *client : clients) {
        client->sendTextMessage(text);
    }
}

QString LocalShareServer::getLocalIpAddress()
{
    for (const QNetworkInterface &interface : QNetworkInterface::allInterfaces()) {
        // Get all address entries *for that interface
        for (const QNetworkAddressEntry &entry : interface.addressEntries()) {
            const QHostAddress &addr = entry.ip();
            // Check if it's an IPv4 address and not a loopback (127.0.0.1)
            if (addr.protocol() == QAbstractSocket::IPv4Protocol &&
                !addr.isLoopback() &&
                !addr.toString().startsWith("169.254.")) // checking for bad addresses
            {
                return addr.toString();
            }
        }
    }
    return QString("Not Found");
}

// --- PRIVATE HANDLERS ---

QHttpServerResponse LocalShareServer::handleMainPage(const QHttpServerRequest &request)
{
    Q_UNUSED(request);
    return QHttpServerResponse("text/html", getEmbeddedWebpage().toUtf8());
}

void LocalShareServer::onNewWebSocketConnection()
{
    QWebSocket *client = webSocketServer->nextPendingConnection();
    if (!client) return;

    qDebug() << "WebSocket client connected:" << client->peerAddress().toString();
    clients.append(client);

    // We don't send anything on connect. We let the
    // ClipboardSyncManager decide what to send.

    connect(client, &QWebSocket::disconnected, this, &LocalShareServer::onWebSocketDisconnected);
}

void LocalShareServer::onWebSocketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        qDebug() << "WebSocket client disconnected";
        clients.removeAll(client);
        client->deleteLater();
    }
}

// =================================================================================
// --- EMBEDDED WEBPAGE (This is the same as before) ---
// =================================================================================

QString LocalShareServer::getEmbeddedWebpage() const
{
    QString wsPort = QString::number(WEBSOCKET_PORT);
    return R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ShowerToys File Share</title>
    <style>
        :root {
            --bg-color: #f4f7f6;
            --card-bg: #ffffff;
            --primary: #007aff;
            --text-color: #333;
            --border-color: #e0e0e0;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
            margin: 0; padding: 1.5em; background: var(--bg-color); color: var(--text-color);
        }
        h2 { margin-top: 0; color: #000; }
        .container {
            max-width: 700px; margin: 0 auto;
        }
        .card {
            background: var(--card-bg);
            border-radius: 12px;
            box-shadow: 0 4px 12px rgba(0,0,0,0.05);
            margin-bottom: 1.5em;
            padding: 1.5em;
        }
        .file-label {
            display: block;
            padding: 12px;
            background: #f9f9f9;
            border: 2px dashed var(--border-color);
            border-radius: 8px;
            text-align: center;
            cursor: pointer;
            font-weight: 500;
        }
        .file-label:hover {
            border-color: var(--primary);
            background: #f0f8ff;
        }
        #fileInput { display: none; }
        #fileList { margin-top: 1em; }
        #fileList div {
            font-size: 0.9em;
            padding: 5px;
        }
        .button {
            display: inline-block; padding: 12px 20px; background: var(--primary);
            color: #fff; text-align: center; border-radius: 8px;
            cursor: pointer; font-weight: 500; text-decoration: none;
            border: none; font-size: 1em;
            width: 100%; box-sizing: border-box;
        }
        .button:hover { background: #0056b3; }
        #uploadStatus {
            margin-top: 1em; font-weight: 500; text-align: center;
            font-size: 0.9em;
        }

        /* Download List Styles */
        #downloadList { list-style: none; padding: 0; }
        #downloadList li {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 12px;
            border-bottom: 1px solid var(--border-color);
        }
        #downloadList li:last-child { border-bottom: none; }
        #downloadList a {
            text-decoration: none;
            color: var(--primary);
            font-weight: 500;
        }
        #downloadList a:hover { text-decoration: underline; }

    </style>
</head>
<body>
    <div class="container">
        <div class="card">
            <h2>Upload to PC</h2>
            <label for="fileInput" class="file-label" id="dropArea">
                Tap to select files, or drag & drop
            </label>
            <input type="file" id="fileInput" multiple> <div id="fileList"></div>
            <button class="button" id="uploadButton">Upload Files</button>
            <div id="uploadStatus"></div>
        </div>

        <div class="card">
            <h2>Download from PC</h2>
            <p style="font-size:0.9em; color:#666;">Files from your PC's "Downloads" folder will appear here.</p>
            <ul id="downloadList">
                <li>(Downloads will appear here...)</li>
            </ul>
        </div>
    </div>

    <script>
        const fileInput = document.getElementById('fileInput');
        const uploadButton = document.getElementById('uploadButton');
        const uploadStatus = document.getElementById('uploadStatus');
        const fileList = document.getElementById('fileList');
        const dropArea = document.getElementById('dropArea');
        let filesToUpload = [];

        // --- File Input & Drag/Drop Logic ---
        function updateFileDisplay() {
            if (filesToUpload.length === 0) {
                fileList.innerHTML = '';
                dropArea.innerText = 'Tap to select files, or drag & drop';
                return;
            }
            dropArea.innerText = `${filesToUpload.length} file(s) selected:`;
            let listHtml = '';
            for (const file of filesToUpload) {
                listHtml += `<div>${file.name} (${(file.size / 1024 / 1024).toFixed(2)} MB)</div>`;
            }
            fileList.innerHTML = listHtml;
        }

        fileInput.onchange = (e) => {
            filesToUpload = Array.from(e.target.files);
            updateFileDisplay();
        };

        ['dragenter', 'dragover', 'dragleave', 'drop'].forEach(eventName => {
            dropArea.addEventListener(eventName, e => {
                e.preventDefault();
                e.stopPropagation();
            }, false);
        });
        ['dragenter', 'dragover'].forEach(eventName => {
            dropArea.addEventListener(eventName, () => dropArea.classList.add('hover'), false);
        });
        ['dragleave', 'drop'].forEach(eventName => {
            dropArea.addEventListener(eventName, () => dropArea.classList.remove('hover'), false);
        });

        dropArea.addEventListener('drop', e => {
            filesToUpload = Array.from(e.dataTransfer.files);
            updateFileDisplay();
        }, false);


        // --- File Upload Logic ---
        uploadButton.onclick = async (e) => {
            e.preventDefault();
            if (filesToUpload.length === 0) {
                uploadStatus.textContent = 'Please select files first.';
                uploadStatus.style.color = 'red';
                return;
            }

            uploadStatus.textContent = `Uploading 1 of ${filesToUpload.length}...`;
            uploadStatus.style.color = 'blue';

            for (let i = 0; i < filesToUpload.length; i++) {
                const file = filesToUpload[i];
                uploadStatus.textContent = `Uploading ${i + 1} of ${filesToUpload.length}: ${file.name}...`;

                // Use an async function to read the file and upload
                try {
                    const base64Data = await readFileAsBase64(file);
                    const response = await fetch('/upload', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({
                            filename: file.name,
                            data: base64Data
                        })
                    });
                    if (!response.ok) {
                        throw new Error(`Upload failed for ${file.name}`);
                    }
                } catch (err) {
                    uploadStatus.textContent = `Error: Could not upload ${file.name}.`;
                    uploadStatus.style.color = 'red';
                    return; // Stop on first error
                }
            }

            uploadStatus.textContent = `Success! ${filesToUpload.length} file(s) uploaded to PC.`;
            uploadStatus.style.color = 'green';
            filesToUpload = [];
            fileInput.value = null; // Clear the input
            updateFileDisplay();
        };

        // Helper function to read file as Base64 using a Promise
        function readFileAsBase64(file) {
            return new Promise((resolve, reject) => {
                const reader = new FileReader();
                reader.readAsDataURL(file);
                reader.onload = (e) => {
                    const base64Data = e.target.result.split(',')[1]; // Get just the data part
                    resolve(base64Data);
                };
                reader.onerror = (e) => {
                    reject(e);
                };
            });
        }

        // --- Download Logic (Placeholder) ---
        const downloadList = document.getElementById('downloadList');
        function loadDownloadableFiles() {
            fetch('/files')
                .then(res => res.json())
                .then(files => {
                    downloadList.innerHTML = ''; // Clear placeholder
                    if (!Array.isArray(files) || files.length === 0) {
                         downloadList.innerHTML = '<li>No files found in Downloads folder.</li>';
                         return;
                    }
                    files.forEach(filename => {
                        const li = document.createElement('li');
                        li.innerHTML = `<span>${filename}</span> <a href="/download/${filename}" download>Download</a>`;
                        downloadList.appendChild(li);
                    });
                })
                .catch(err => {
                    console.error(err);
                    downloadList.innerHTML = '<li>Error loading files.</li>';
                });
        }

        // Load files when the page opens
        loadDownloadableFiles();

    </script>
</body>
</html>
    )";
}


