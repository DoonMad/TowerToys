#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <AppController/appcontroller.h>
#include <MacroManager/macromanager.h>
#include <ClipboardSync/clipboardsyncmanager.h>
#include <QHash>
#include <QString>
#include <moduleinfo.h>
#include <QWidget>
#include <modulemanager.h>
#include <QStackedWidget>

class LocalShareServer;
class FileShareManager;
class QPushButton;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:


private:
    Ui::MainWindow *ui;
    AppController *controller;
    MacroManager *macroManager;
    ModuleManager *moduleManager;
    LocalShareServer* shareServer;
    FileShareManager* fileShareManager;
    ClipboardSyncManager* clipboardSyncManager;
    QMap<QString, ModuleInfo> moduleInfoMap;
    QMap<QString, QWidget*> loadedWidgets;
    QStackedWidget *stackWidget;
    QHash<QString, QPushButton*> moduleButtons;

    void updateButtonSelection(QPushButton* selectedButton);
    void onModuleButtonClicked(const QString& moduleName);
    QWidget* createWidgetForModule(const QString& moduleName);
    void setupDynamicUI();
};
#endif // MAINWINDOW_H
