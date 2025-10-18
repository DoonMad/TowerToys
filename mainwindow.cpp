#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QMessageBox"
#include "QPushButton"
#include "MacroManager/macromanager.h"
#include "MacroManager/macro.h"
#include "AppController/appcontroller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    controller = new AppController(this, this);
    controller->start();

    MacroManager *manager = new MacroManager(this);
    connect(manager, &MacroManager::macroAdded, this, [=](QString name){
        ui->listWidget->addItem(name);
    });

    connect(ui->addMacroButton, &QPushButton::clicked, this, [=](){
        Macro *macro = new Macro(this);
        macro->name = ui->lineEdit->text();

        if(!macro->name.isEmpty()){
            manager->addMacro(*macro);
        }
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked(){
    QMessageBox::information(this, "Showertoys", "Hello from ShowerToys!");
}
