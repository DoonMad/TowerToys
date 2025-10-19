#include "addactiondialog.h"
#include "ui_addactiondialog.h"

AddActionDialog::AddActionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddActionDialog)
{
    ui->setupUi(this);

    ui->actionTypeCombo->addItems({"Open App", "Open URL", "Type Keystroke"});
    connect(ui->actionTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), ui->stackedWidget, &QStackedWidget::setCurrentIndex);
}

AddActionDialog::~AddActionDialog()
{
    delete ui;
}
