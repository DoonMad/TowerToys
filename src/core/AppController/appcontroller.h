#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMainWindow>

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QMainWindow * window, QObject *parent = nullptr);
    void start();

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QMainWindow *mainWindow;

    void setupTray();
    void setupBackgroundTasks();

signals:
};

#endif // APPCONTROLLER_H
