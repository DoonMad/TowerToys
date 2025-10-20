#include "addactiondialog.h"
#include "ui_addactiondialog.h"
#include "Actions/openappaction.h"
#include "Actions/openurlaction.h"
#include "Actions/typekeystrokeaction.h"
#include <QFileDialog>

AddActionDialog::AddActionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddActionDialog)
{
    ui->setupUi(this);

    ui->actionTypeCombo->addItems({"Open App", "Open URL", "Type Keystroke"});
    connect(ui->actionTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), ui->stackedWidget, &QStackedWidget::setCurrentIndex);
    ui->stackedWidget->setCurrentIndex(0);
}

QSharedPointer<Action> AddActionDialog::getAction() const
{
    int index = ui->actionTypeCombo->currentIndex();
    QString actionType = ui->actionTypeCombo->itemText(index);

    if (actionType == "Open App")
    {
        QString path = ui->appPathEdit->text();
        if (path.isEmpty()) return nullptr;
        return QSharedPointer<OpenAppAction>::create(path);
    }
    else if (actionType == "Open URL")
    {
        QString url = ui->urlEdit->text();
        if (url.isEmpty()) return nullptr;
        // Simple check: add http:// if missing
        if (!url.startsWith("http://") && !url.startsWith("https://")) {
            url = "http://" + url;
        }
        return QSharedPointer<OpenURLAction>::create(url);
    }
    else if (actionType == "Type Keystroke")
    {
        QString text = ui->keystrokeEdit->text();
        if (text.isEmpty()) return nullptr;
        return QSharedPointer<TypeKeystrokeAction>::create(text);
    }

    return nullptr;
}

void AddActionDialog::on_browseAppButton_clicked()
{
    // Open a file dialog to find .exe files
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select Application",
        QString(), // Start in default directory
        "Applications (*.exe);;All Files (*)"
        );

    if (!filePath.isEmpty()) {
        ui->appPathEdit->setText(filePath);
    }
}

AddActionDialog::~AddActionDialog()
{
    delete ui;
}
