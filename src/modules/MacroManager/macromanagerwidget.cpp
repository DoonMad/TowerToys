#include "macromanagerwidget.h"
#include "ui_macromanagerwidget.h"

MacroManagerWidget::MacroManagerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MacroManagerWidget)
{
    ui->setupUi(this);
}

MacroManagerWidget::~MacroManagerWidget()
{
    delete ui;
}
