#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <AppController/appcontroller.h>
#include <MacroManager/macromanager.h>
#include <QHash>
#include <QString>
#include <moduleinfo.h>
#include <QWidget>
#include <modulemanager.h>
#include <QStackedWidget>

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
    QHash<QString, ModuleInfo> moduleInfoMap;
    QHash<QString, QWidget*> loadedWidgets;
    QStackedWidget *stackWidget;

    void onModuleButtonClicked(const QString& moduleName);
    QWidget* createWidgetForModule(const QString& moduleName);
    void setupDynamicUI();
};
#endif // MAINWINDOW_H
