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
    connect(manager, &MacroManager::macroRemoved, this, &MacroManagerWidget::onMacroRemoved);
    connect(manager, &MacroManager::hotkeyStatus, this, &MacroManagerWidget::onHotkeyStatus);

    ui->rightPanel->setEnabled(false);
}

void MacroManagerWidget::onHotkeyStatus(QString macroName, bool success, QString message)
{
    // Only show status for the macro we are currently editing
    if (currentMacro && currentMacro->name == macroName) {
        ui->hotkeyErrorLabel->setText(message);

        // Set the label color (red for error, green for success)
        if (success) {
            ui->hotkeyErrorLabel->setStyleSheet("color: green;");
        } else {
            ui->hotkeyErrorLabel->setStyleSheet("color: red;");
        }
    }
}

void MacroManagerWidget::onMacroAdded(QSharedPointer<Macro> macro)
{
    QListWidgetItem* item = new QListWidgetItem(macro->name, ui->macroListWidget);

    // This is the key: store the actual pointer inside the UI item
    // QVariant is like a box which can store anything. We want to store the shared pointer, but the function doesn't know that.
    // That's why it accepts QVariant, which is like a universal box in which we can store anything and pass inside setData();
    item->setData(Qt::UserRole, QVariant::fromValue(macro));

    ui->macroListWidget->setCurrentItem(item);
}

void MacroManagerWidget::on_addMacroButton_clicked()
{
    QSharedPointer<Macro> macro = QSharedPointer<Macro>::create();
    macro->name = "New Macro"; // default name

    // Tell the manager to add it. The manager will emit `macroAdded`,
    // which `onMacroAdded` slot will catch, updating the UI.
    manager->addMacro(macro);
}

void MacroManagerWidget::on_macroListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous); // don't care about the last item

    if (current) {
        // Retrieve the pointer stored in onMacroAdded
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
        emit manager->macroEdited(currentMacro);
    }
}

void MacroManagerWidget::on_macroHotkeyEdit_keySequenceChanged(const QKeySequence &keySequence){
    if(currentMacro){
        currentMacro->shortcut = keySequence.toString(QKeySequence::NativeText);
        emit manager->macroEdited(currentMacro);
    }
}

void MacroManagerWidget::on_addActionButton_clicked()
{
    if (!currentMacro) return; // Safety check

    AddActionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QSharedPointer<Action> newAction = dialog.getAction();
        if (newAction) {
            currentMacro->addAction(newAction);
            updateActionList(); // Refresh the list
        }
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
    // 3. Call manager->removeMacro(macro)
    // 4. Implement the onMacroRemoved slot to remove item from list

    QListWidgetItem* currentItem = ui->macroListWidget->currentItem();
    if (!currentItem) return; // Nothing selected

    // Get the macro pointer from the item
    QSharedPointer<Macro> macro = currentItem->data(Qt::UserRole).value<QSharedPointer<Macro>>();

    if (macro) {
        // Tell the manager to remove it. The manager will
        // emit macroRemoved, which onMacroRemoved slot
        // will catch, updating the UI.
        manager->removeMacro(macro);
    }
}

void MacroManagerWidget::on_removeActionButton_clicked()
{
    // TODO: Implement this
    // 1. Get selected action from ui->actionListWidget
    // 2. Remove it from currentMacro->actions
    // 3. Remove it from the ui->actionListWidget

    if (!currentMacro) return;

    int currentRow = ui->actionListWidget->currentRow();
    if (currentRow < 0) return;

    QListWidgetItem* item = ui->actionListWidget->item(currentRow);
    QSharedPointer<Action> action = item->data(Qt::UserRole).value<QSharedPointer<Action>>();

    if (action) {
        currentMacro->removeAction(action);
        delete ui->actionListWidget->takeItem(currentRow);
    }
}

void MacroManagerWidget::onMacroRemoved(QSharedPointer<Macro> macro)
{
    // Find the item in the list that matches this macro pointer
    for (int i = 0; i < ui->macroListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->macroListWidget->item(i);
        QSharedPointer<Macro> itemMacro = item->data(Qt::UserRole).value<QSharedPointer<Macro>>();

        if (itemMacro == macro) {
            // found it. Remove it from the list and delete it.
            delete ui->macroListWidget->takeItem(i);
            break;
        }
    }
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
        ui->hotkeyErrorLabel->clear();
    } else {
        // A macro is selected: enable panel and fill fields
        ui->rightPanel->setEnabled(true);
        ui->macroNameEdit->setText(currentMacro->name);
        ui->macroHotkeyEdit->setKeySequence(QKeySequence::fromString(currentMacro->shortcut));
        updateActionList();
        ui->hotkeyErrorLabel->clear();
    }

}

void MacroManagerWidget::updateActionList()
{
    ui->actionListWidget->clear();
    if (currentMacro) {
        for (const auto& action : currentMacro->actions) {
            // ui->actionListWidget->addItem(action->description());
            // We'll also need to store the Action pointer in this list's items
            // to make 'remove' work later.
            QListWidgetItem* item = new QListWidgetItem(action->description());
            item->setData(Qt::UserRole, QVariant::fromValue(action));
            ui->actionListWidget->addItem(item);
        }
    }
}

MacroManagerWidget::~MacroManagerWidget()
{
    delete ui;
}
