#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("DoonMadOrg");
    QCoreApplication::setApplicationName("ShowerToys");

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    a.setWindowIcon(QIcon(":/resources/icons/app_icon.png"));

    MainWindow w;
    w.show();

    return a.exec();
}
