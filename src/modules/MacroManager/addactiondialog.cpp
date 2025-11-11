#include "addactiondialog.h"
#include "ui_addactiondialog.h"
#include "Actions/openappaction.h"
#include "Actions/openurlaction.h"
#include "Actions/typekeystrokeaction.h"
#include "Actions/openvscodefolderaction.h"
#include "Actions/openfolderaction.h"
#include "Actions/runcommandaction.h"
#include <QFileDialog>

AddActionDialog::AddActionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddActionDialog)
{
    ui->setupUi(this);

    ui->actionTypeCombo->addItems({"Open App",
                                   "Open URL",
                                   "Type Keystroke",
                                   "Open Folder in VS Code",
                                   "Open Folder",
                                   "Run Command"
    });
    connect(ui->actionTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), ui->stackedWidget, &QStackedWidget::setCurrentIndex);
    ui->stackedWidget->setCurrentIndex(0);
}

QSharedPointer<Action> AddActionDialog::getAction() const
{
    int index = ui->actionTypeCombo->currentIndex();
    QString actionType = ui->actionTypeCombo->itemText(index);

    if (editingAction) {
        qDebug() << "edit ho rhi hai action";
        // Use qSharedPointerDynamicCast to check the type and call the appropriate setter
        if (actionType == "Open App") {
            auto openApp = qSharedPointerDynamicCast<OpenAppAction>(editingAction);
            QString path = ui->appPathEdit->text();
            if (openApp && !path.isEmpty()) {
                openApp->setPath(path);
            } else { return nullptr; }
        } else if (actionType == "Open URL") {
            auto openUrl = qSharedPointerDynamicCast<OpenURLAction>(editingAction);
            QString url = ui->urlEdit->text();
            if (openUrl && !url.isEmpty()) {
                // Add http:// prefix if missing
                if (!url.startsWith("http://") && !url.startsWith("https://")) {
                    url = "http://" + url;
                }
                openUrl->setUrl(url);
            } else { return nullptr; }
        } else if (actionType == "Type Keystroke") {
            auto typeKey = qSharedPointerDynamicCast<TypeKeystrokeAction>(editingAction);
            QString text = ui->keystrokeEdit->text();
            if (typeKey && !text.isEmpty()) {
                typeKey->setText(text);
            } else { return nullptr; }
        } else if (actionType == "Open Folder in VS Code") {
            auto openVSCode = qSharedPointerDynamicCast<OpenVSCodeFolderAction>(editingAction);
            QString path = ui->vsCodeFolderPathEdit->text();
            if (openVSCode && !path.isEmpty()) {
                openVSCode->setPath(path);
            } else { return nullptr; }
        } else if (actionType == "Open Folder") {
            auto openFolder = qSharedPointerDynamicCast<OpenFolderAction>(editingAction);
            QString path = ui->folderPathEdit->text();
            if (openFolder && !path.isEmpty()) { openFolder->setPath(path); }
            else { return nullptr; }
        }
        else if (actionType == "Run Command") {
            auto runCmd = qSharedPointerDynamicCast<RunCommandAction>(editingAction);
            QString cmd = ui->commandLineEdit->text();
            if (runCmd && !cmd.isEmpty()) { runCmd->setCommand(cmd); }
            else { return nullptr; }
        } else {
            qWarning() << "getAction (Edit Mode): Unknown action type!";
            return nullptr; // Unknown type during edit
        }

        return editingAction; // Return the MODIFIED existing action pointer

    } else {

        if (actionType == "Open App") {
            QString path = ui->appPathEdit->text();
            if (path.isEmpty()) return nullptr;
            return QSharedPointer<OpenAppAction>::create(path);
        } else if (actionType == "Open URL") {
            QString url = ui->urlEdit->text();
            if (url.isEmpty()) return nullptr;
            if (!url.startsWith("http://") && !url.startsWith("https://")) {
                url = "http://" + url;
            }
            return QSharedPointer<OpenURLAction>::create(url);
        } else if (actionType == "Type Keystroke") {
            QString text = ui->keystrokeEdit->text();
            if (text.isEmpty()) return nullptr;
            return QSharedPointer<TypeKeystrokeAction>::create(text);
        } else if (actionType == "Open Folder in VS Code") {
            QString path = ui->vsCodeFolderPathEdit->text();
            if (path.isEmpty()) return nullptr;
            return QSharedPointer<OpenVSCodeFolderAction>::create(path);
        } else if (actionType == "Open Folder") {
            QString path = ui->folderPathEdit->text();
            if (path.isEmpty()) return nullptr;
            return QSharedPointer<OpenFolderAction>::create(path);
        }
        else if (actionType == "Run Command") {
            QString cmd = ui->commandLineEdit->text();
            if (cmd.isEmpty()) return nullptr;
            return QSharedPointer<RunCommandAction>::create(cmd);
        } else {
            qWarning() << "getAction (Create Mode): Unknown action type!";
            return nullptr;
        }
    }

    return nullptr;
}

void AddActionDialog::setAction(QSharedPointer<Action> actionToEdit)
{
    if (!actionToEdit) {
        qWarning() << "setAction called with null actionToEdit.";
        editingAction = nullptr;
        // Optionally, reset UI to default "create new action" state if action is null
        ui->actionTypeCombo->setEnabled(true);
        ui->actionTypeCombo->setCurrentIndex(0);
        ui->appPathEdit->clear();
        ui->urlEdit->clear();
        ui->keystrokeEdit->clear();
        ui->vsCodeFolderPathEdit->clear();
        ui->folderPathEdit->clear();
        ui->commandLineEdit->clear();
        return;
    }

    editingAction = actionToEdit;

    // --- IMPORTANT: Clear all input fields first to prevent old values from showing ---
    ui->appPathEdit->clear();
    ui->urlEdit->clear();
    ui->keystrokeEdit->clear();
    ui->vsCodeFolderPathEdit->clear();
    ui->folderPathEdit->clear();
    ui->commandLineEdit->clear();

    // qWarning() << "setAction received action of type:" << actionToEdit->description(); // Debugging

    // --- Set combo box and stacked widget, then populate fields ---
    if (auto openApp = qSharedPointerDynamicCast<OpenAppAction>(actionToEdit)) {
        ui->actionTypeCombo->setCurrentText("Open App");
        ui->stackedWidget->setCurrentIndex(0);
        ui->appPathEdit->setText(openApp->getPath());
    } else if (auto openUrl = qSharedPointerDynamicCast<OpenURLAction>(actionToEdit)) {
        ui->actionTypeCombo->setCurrentText("Open URL");
        ui->stackedWidget->setCurrentIndex(1);
        ui->urlEdit->setText(openUrl->getUrl());
    } else if (auto typeKey = qSharedPointerDynamicCast<TypeKeystrokeAction>(actionToEdit)) {
        ui->actionTypeCombo->setCurrentText("Type Keystroke");
        ui->stackedWidget->setCurrentIndex(2);
        ui->keystrokeEdit->setText(typeKey->getText());
    } else if (auto openVSCode = qSharedPointerDynamicCast<OpenVSCodeFolderAction>(actionToEdit)) {
        ui->actionTypeCombo->setCurrentText("Open Folder in VS Code");
        ui->stackedWidget->setCurrentIndex(3);
        ui->vsCodeFolderPathEdit->setText(openVSCode->getPath());
    } else if (auto openFolder = qSharedPointerDynamicCast<OpenFolderAction>(actionToEdit)) {
        ui->actionTypeCombo->setCurrentText("Open Folder");
        ui->stackedWidget->setCurrentIndex(4);
        ui->folderPathEdit->setText(openFolder->getPath());
    } else if (auto runCmd = qSharedPointerDynamicCast<RunCommandAction>(actionToEdit)) {
        ui->actionTypeCombo->setCurrentText("Run Command");
        ui->stackedWidget->setCurrentIndex(5);
        ui->commandLineEdit->setText(runCmd->getCommand());
    } else {
        qWarning() << "AddActionDialog::setAction - Unknown action type for editing!";
        editingAction = nullptr;
        // Optionally reset UI to default state if type is unknown
        ui->actionTypeCombo->setEnabled(true);
        ui->actionTypeCombo->setCurrentIndex(0);
        return;
    }

    // Disable changing the type when editing a specific action
    ui->actionTypeCombo->setEnabled(false);
}

void AddActionDialog::on_browseAppButton_clicked()
{
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

void AddActionDialog::on_browseVSCodeFolderButton_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(
        this,
        "Select Folder to Open in VS Code",
        QString(), // Start in default directory
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!dirPath.isEmpty()) {
        ui->vsCodeFolderPathEdit->setText(dirPath);
    }
}

void AddActionDialog::on_browseFolderButton_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(
        this,
        "Select Folder",
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
    if (!dirPath.isEmpty()) {
        ui->folderPathEdit->setText(dirPath);
    }
}

AddActionDialog::~AddActionDialog()
{
    delete ui;
}
