#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QMessageBox"
#include "QPushButton"
#include "MacroManager/macromanager.h"
#include "AppController/appcontroller.h"
#include <QSharedPointer>

#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include "modulemanager.h"
#include <QHash>
#include <QString>
#include <MacroManager/macromanagerwidget.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    controller = new AppController(this, this);
    controller->start();

    macroManager = controller->getMacroManager();
    moduleManager = new ModuleManager(this);
    moduleInfoMap = moduleManager->loadModules();

    setupDynamicUI();


    // MacroManager *manager = new MacroManager(this);
    // connect(manager, &MacroManager::macroAdded, this, [=](QSharedPointer<Macro> macro){
    //     ui->listWidget->addItem(macro->name);
    // });

    // connect(ui->addMacroButton, &QPushButton::clicked, this, [=](){
    //     QSharedPointer<Macro> macro = QSharedPointer<Macro>::create();
    //     macro->name = ui->lineEdit->text();
    //     if(!macro->name.isEmpty()){
    //         manager->addMacro(macro);
    //     }
    // });

}

void MainWindow::setupDynamicUI() {
    QWidget *central = ui->centralwidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    QFrame *sidePanel = new QFrame;
    sidePanel->setFixedWidth(200);
    QVBoxLayout *sideLayout = new QVBoxLayout(sidePanel);

    stackWidget = new QStackedWidget;

    for (const ModuleInfo& moduleInfo: moduleInfoMap)
    {
        const QString moduleName = moduleInfo.name;
        QPushButton* button = new QPushButton(moduleName, this);
        button->setIcon(QIcon(moduleInfo.iconPath));
        sideLayout->addWidget(button);

        connect(button, &QPushButton::clicked, this, [this, moduleName]() {
            onModuleButtonClicked(moduleName);
        });
    }

    sideLayout->addStretch();
    mainLayout->addWidget(sidePanel);
    mainLayout->addWidget(stackWidget);
    setCentralWidget(central);
}

void MainWindow::onModuleButtonClicked(const QString& moduleName)
{
    QWidget* widgetToShow = nullptr;

    // if widget already exists
    if (loadedWidgets.contains(moduleName)){
        widgetToShow = loadedWidgets.value(moduleName);
    }
    else{
        // create widget object if it doesn't exist
        widgetToShow = createWidgetForModule(moduleName);

        if (widgetToShow){
            stackWidget->addWidget(widgetToShow);
            loadedWidgets.insert(moduleName, widgetToShow);
        }
    }

    if (widgetToShow){
        stackWidget->setCurrentWidget(widgetToShow);
    }
}

QWidget* MainWindow::createWidgetForModule(const QString& moduleName)
{
    // Get moduleInfo from moduleName
    const ModuleInfo& info = moduleInfoMap.value(moduleName);

    if (info.widgetClass == "MacroManagerWidget")
    {
        return new MacroManagerWidget(macroManager, info, this);
    }

    if (info.widgetClass == "FileShareWidget")
    {
        // return new FileShareWidget(m_fileShareManager, info, this);
        return new QWidget(this); // Placeholder for now
    }

    if (info.widgetClass == "ClipboardSyncWidget")
    {
        // return new ClipboardSyncWidget(m_clipboardManager, info, this);
        return new QWidget(this); // Placeholder for now
    }

    return nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}
