#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMainWindow>
#include <qhotkey.h>
#include "MacroManager/macromanager.h"
#include <HotkeyManager/hotkeymanager.h>

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QMainWindow * window, QObject *parent = nullptr);
    void start();
    MacroManager* getMacroManager() const { return macroManager; }

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QMainWindow *mainWindow;
    HotkeyManager* hotkeyManager;
    MacroManager* macroManager;

    void setupTray();
    void setupBackgroundTasks();

signals:
};

#endif // APPCONTROLLER_H
