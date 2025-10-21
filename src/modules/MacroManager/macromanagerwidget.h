#ifndef MACROMANAGERWIDGET_H
#define MACROMANAGERWIDGET_H

#include <QWidget>
#include "macromanager.h"
#include "../moduleinfo.h"
#include <QListWidgetItem>
#include "macro.h"

namespace Ui {
class MacroManagerWidget;
}

class MacroManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MacroManagerWidget(MacroManager* manager, const ModuleInfo& info, QWidget *parent = nullptr);
    ~MacroManagerWidget();

private:
    Ui::MacroManagerWidget *ui;
    MacroManager* manager;
    QSharedPointer<Macro> currentMacro;

    void updateDetailPane();    // Helper to refresh the right side
    void updateActionList();

private slots:
    void onMacroAdded(QSharedPointer<Macro> macro);
    void onMacroRemoved(QSharedPointer<Macro> macro);

    void on_addMacroButton_clicked();
    void on_macroListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_macroNameEdit_textChanged(const QString &text);
    void on_addActionButton_clicked();
    void on_executeMacroButton_clicked();
    void on_removeMacroButton_clicked();
    void on_removeActionButton_clicked();

};

#endif // MACROMANAGERWIDGET_H
