#include "appcontroller.h"
#include <QIcon>
#include <QMenu>
#include <QDebug>
#include <QApplication>
#include <QGuiApplication> // ADD FOR CLIPBOARD
#include <QClipboard>
#include <QMimeData>
#include <QSettings>

AppController::AppController(QMainWindow *window, QObject *parent)
    : QObject(parent), mainWindow(window)
{
    macroManager = new MacroManager(this);
    hotkeyManager = new HotkeyManager(this);
    shareServer = new LocalShareServer(this);
    fileShareManager = new FileShareManager(this);
    clipboardSyncManager = new ClipboardSyncManager(this);

    shareServer->setFileShareManager(fileShareManager);
    shareServer->setClipboardSyncManager(clipboardSyncManager);

    QSettings settings;
    clipboardSyncEnabled = settings.value("clipboardSyncEnabled", true).toBool();

    connect(macroManager, &MacroManager::macroAdded, hotkeyManager, &HotkeyManager::registerMacro);
    connect(macroManager, &MacroManager::macroRemoved, hotkeyManager, &HotkeyManager::unregisterMacro);
    connect(macroManager, &MacroManager::macroEdited, hotkeyManager, &HotkeyManager::reregisterMacro);
    connect(hotkeyManager, &HotkeyManager::hotkeyStatusChanged, macroManager, [this](QSharedPointer<Macro> macro, bool success, QString message) {
        emit macroManager->hotkeyStatus(macro->name, success, message);
    });

    connect(shareServer, &LocalShareServer::fileUploadRequest, fileShareManager, &FileShareManager::onFileUploadRequest);
    // connect(shareServer, &LocalShareServer::fileListRequest, fileShareManager, &FileShareManager::onFileListRequest);
    // connect(shareServer, &LocalShareServer::fileDownloadRequest, fileShareManager, &FileShareManager::onFileDownloadRequest);

    connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &AppController::onSystemClipboardChanged);
    // connect(shareServer, &LocalShareServer::clipboardDataRequest, clipboardSyncManager, &ClipboardSyncManager::handleClipboardDataRequest);
}

void AppController::start()
{
    setupTray();
}

bool AppController::isClipboardSyncEnabled() const
{
    return clipboardSyncEnabled;
}

void AppController::setClipboardSyncEnabled(bool enabled)
{
    if (clipboardSyncEnabled != enabled) {
        clipboardSyncEnabled = enabled;
        qDebug() << "AppController: Clipboard Sync set to" << enabled;

        // --- SAVE SETTING ---
        QSettings settings;
        settings.setValue("clipboardSyncEnabled", clipboardSyncEnabled); // <-- SAVE VALUE

        // Optional: If disabling, maybe clear the last pushed text?
        // if (!enabled) {
        //    m_clipboardSyncManager->updateClipboardText("");
        //    m_shareServer->pushTextToClients("");
        // }
    }
}

void AppController::onSystemClipboardChanged()
{
    // --- ADD CHECK ---
    if (!clipboardSyncEnabled) { // <-- CHECK IF ENABLED
        return; // Do nothing if sync is disabled
    }

    QClipboard *clipboard = QGuiApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData->hasText()) {
        QString text = clipboard->text();
        qDebug() << "AppController: Clipboard changed (text, SYNCING):" << text.left(50) << "...";
        clipboardSyncManager->updateClipboardText(text);
        shareServer->pushTextToClients(text);
    }
}

void AppController::setupTray(){
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/resources/icons/app_icon.png"));
    trayIcon->setToolTip("TowerToys");

    trayMenu = new QMenu(nullptr);
    trayMenu->addAction("Open", this, [=](){
        mainWindow->show();
    });
    trayMenu->addAction("Exit", qApp, &QApplication::quit);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}


