#include "clipboardsyncwidget.h"
#include "ui_clipboardsyncwidget.h"
#include "clipboardsyncmanager.h"
#include "AppController/appcontroller.h"

ClipboardSyncWidget::ClipboardSyncWidget(AppController* controller, // <-- ADD controller
                                         ClipboardSyncManager* manager,
                                         const ModuleInfo& info,
                                         QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClipboardSyncWidget),
    manager(manager),
    controller(controller)
{
    ui->setupUi(this);

    // Set title and description
    ui->titleLabel->setText(info.name);
    ui->descriptionLabel->setText(info.description);

    // TODO: Get initial state of checkbox from settings/AppController
    // TODO: Connect checkbox signal to AppController slot
    if (controller) { // Safety check
        // Get initial state from AppController
        ui->enableClipboardSyncCheckbox->setChecked(controller->isClipboardSyncEnabled());

        // Connect checkbox toggle signal to AppController's slot
        connect(ui->enableClipboardSyncCheckbox, &QCheckBox::toggled,
                controller, &AppController::setClipboardSyncEnabled);
    } else {
        // Disable checkbox if controller is somehow missing
        ui->enableClipboardSyncCheckbox->setEnabled(false);
    }
}

ClipboardSyncWidget::~ClipboardSyncWidget()
{
    delete ui;
}

void ClipboardSyncWidget::on_enableClipboardSyncCheckbox_toggled(bool checked)
{
    qDebug() << "Clipboard Sync Enable toggled:" << checked;
    // TODO: Tell AppController to enable/disable clipboard monitoring
}

