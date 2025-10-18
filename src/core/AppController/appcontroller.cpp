#include "appcontroller.h"
#include <QIcon>
#include <QMenu>
#include <QDebug>
#include <QApplication>

AppController::AppController(QMainWindow *window, QObject *parent)
    : QObject(parent), mainWindow(window)
{

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
