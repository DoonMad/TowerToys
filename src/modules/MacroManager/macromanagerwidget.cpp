#include "macromanagerwidget.h"
#include "ui_macromanagerwidget.h"
#include "macromanager.h"
#include "macro.h"
#include "addactiondialog.h" // We'll need this for the "Add Action" button
#include <QListWidgetItem>
#include <QVariant>

MacroManagerWidget::MacroManagerWidget(MacroManager* manager, const ModuleInfo& info, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MacroManagerWidget)
    , manager(manager)
{
    ui->setupUi(this);

    ui->titleLabel->setText(info.name);
    ui->descriptionLabel->setText(info.description);

    connect(manager, &MacroManager::macroAdded, this, &MacroManagerWidget::onMacroAdded);

    ui->rightPanel->setEnabled(false);
}

void MacroManagerWidget::onMacroAdded(QSharedPointer<Macro> macro)
{
    QListWidgetItem* item = new QListWidgetItem(macro->name, ui->macroListWidget);

    // This is the key: store the actual pointer inside the UI item
    item->setData(Qt::UserRole, QVariant::fromValue(macro));

    ui->macroListWidget->setCurrentItem(item);
}

void MacroManagerWidget::on_addMacroButton_clicked()
{
    QSharedPointer<Macro> macro = QSharedPointer<Macro>::create();
    macro->name = "New Macro"; // Give it a default name

    // Tell the manager to add it. The manager will emit `macroAdded`,
    // which our `onMacroAdded` slot will catch, updating the UI.
    manager->addMacro(macro);
}

void MacroManagerWidget::on_macroListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous); // We don't care about the last item

    if (current) {
        // Retrieve the pointer we stored in onMacroAdded
        currentMacro = current->data(Qt::UserRole).value<QSharedPointer<Macro>>();
    } else {
        currentMacro.clear(); // Clear the pointer if nothing is selected
    }

    // Refresh the entire right side of the UI
    updateDetailPane();
}

void MacroManagerWidget::on_macroNameEdit_textChanged(const QString &text)
{
    // Update the C++ object and the list item text in real-time
    if (currentMacro && ui->macroListWidget->currentItem()) {
        currentMacro->name = text;
        ui->macroListWidget->currentItem()->setText(text);
    }
}

void MacroManagerWidget::on_addActionButton_clicked()
{
    if (!currentMacro) return; // Safety check

    AddActionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        // --- TODO: Implement getAction() in AddActionDialog ---
        // QSharedPointer<Action> newAction = dialog.getAction();
        // if (newAction) {
        //     currentMacro->addAction(newAction);
        //     updateActionList(); // Refresh the list
        // }
    }
}

void MacroManagerWidget::on_executeMacroButton_clicked()
{
    if (currentMacro) {
        currentMacro->execute();
    }
}

void MacroManagerWidget::on_removeMacroButton_clicked()
{
    // TODO: Implement this
    // 1. Get current item
    // 2. Get QSharedPointer<Macro> from its data
    // 3. Call m_manager->removeMacro(macro)
    // 4. Implement the onMacroRemoved slot to remove item from list
}

void MacroManagerWidget::on_removeActionButton_clicked()
{
    // TODO: Implement this
    // 1. Get selected action from ui->actionListWidget
    // 2. Remove it from currentMacro->actions
    // 3. Remove it from the ui->actionListWidget
}


// --- Helper Functions ---

void MacroManagerWidget::updateDetailPane()
{
    if (!currentMacro) {
        // No macro selected: disable panel and clear fields
        ui->rightPanel->setEnabled(false);
        ui->macroNameEdit->clear();
        ui->macroHotkeyEdit->clear();
        ui->actionListWidget->clear();
    } else {
        // A macro is selected: enable panel and fill fields
        ui->rightPanel->setEnabled(true);
        ui->macroNameEdit->setText(currentMacro->name);
        ui->macroHotkeyEdit->setText(currentMacro->shortcut); // Assuming Macro has 'shortcut'
        updateActionList();
    }
}

void MacroManagerWidget::updateActionList()
{
    ui->actionListWidget->clear();
    if (currentMacro) {
        for (const auto& action : currentMacro->actions) {
            ui->actionListWidget->addItem(action->description());
            // We'll also need to store the Action pointer in this list's items
            // to make 'remove' work later.
        }
    }
}

MacroManagerWidget::~MacroManagerWidget()
{
    delete ui;
}
