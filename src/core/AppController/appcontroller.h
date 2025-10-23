#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMainWindow>
#include <qhotkey.h>
#include "MacroManager/macromanager.h"
#include <HotkeyManager/hotkeymanager.h>
#include <LocalShareServer/localshareserver.h>
#include <FileShare/filesharemanager.h>
#include <ClipboardSync/clipboardsyncmanager.h>

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QMainWindow * window, QObject *parent = nullptr);
    void start();
    MacroManager* getMacroManager() const { return macroManager; }
    LocalShareServer* getShareServer() const { return shareServer; }
    FileShareManager* getFileShareManager() const { return fileShareManager; }
    ClipboardSyncManager* getClipboardSyncManager() const { return clipboardSyncManager; }
    bool isClipboardSyncEnabled() const;

public slots:
    void setClipboardSyncEnabled(bool enabled);

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QMainWindow *mainWindow;
    HotkeyManager* hotkeyManager;
    MacroManager* macroManager;
    LocalShareServer* shareServer;
    FileShareManager* fileShareManager;
    ClipboardSyncManager* clipboardSyncManager;
    bool clipboardSyncEnabled = true;

    void setupTray();
    void setupBackgroundTasks();

private slots:
    void onSystemClipboardChanged();

signals:
};

#endif // APPCONTROLLER_H
