#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QSharedMemory>
#include <QFile>
#include <windows.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("DoonMadOrg");
    QCoreApplication::setApplicationName("TowerToys");

    // Single Instance Check
    QSharedMemory shared("TowerToys_SingleInstance_Key");
    if (!shared.create(512, QSharedMemory::ReadWrite)) {
        // App is already running, find the window and bring it to front
        HWND hwnd = FindWindowW(NULL, L"TowerToys"); 
        if (hwnd) {
            ShowWindow(hwnd, SW_RESTORE);
            SetForegroundWindow(hwnd);
        }
        return 0; // Exit this new instance
    }

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    // Load and Apply Global Stylesheet
    QFile styleFile(":/resources/styles/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(styleFile.readAll());
        a.setStyleSheet(style);
        styleFile.close();
    }

    a.setWindowIcon(QIcon(":/resources/icons/app_icon.png"));

    MainWindow w;
    w.show();

    return a.exec();
}
