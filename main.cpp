#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("DoonMadOrg");
    QCoreApplication::setApplicationName("ShowerToys");


    QApplication::setStyle(QStyleFactory::create("windowsvista"));

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.show();

    return a.exec();
}
