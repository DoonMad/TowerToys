#include "appcontroller.h"
#include <QIcon>
#include <QMenu>
#include <QDebug>
#include <QApplication>

AppController::AppController(QMainWindow *window, QObject *parent)
    : QObject(parent), mainWindow(window)
{
    macroManager = new MacroManager(this);
    hotkeyManager = new HotkeyManager(this);

    connect(macroManager, &MacroManager::macroAdded, hotkeyManager, &HotkeyManager::registerMacro);
    connect(macroManager, &MacroManager::macroRemoved, hotkeyManager, &HotkeyManager::unregisterMacro);
    connect(macroManager, &MacroManager::macroEdited, hotkeyManager, &HotkeyManager::reregisterMacro);
    connect(hotkeyManager, &HotkeyManager::hotkeyStatusChanged, macroManager, [this](QSharedPointer<Macro> macro, bool success, QString message) {
        emit macroManager->hotkeyStatus(macro->name, success, message);
    });
}

void AppController::start()
{
    setupTray();
}

void AppController::setupTray(){
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/resources/icons/app_icon.png"));
    trayIcon->setToolTip("ShowerToys");

    trayMenu = new QMenu(nullptr);
    trayMenu->addAction("Open", this, [=](){
        mainWindow->show();
    });
    trayMenu->addAction("Exit", qApp, &QApplication::quit);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}
