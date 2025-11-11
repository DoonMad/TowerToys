#ifndef ADDACTIONDIALOG_H
#define ADDACTIONDIALOG_H

#include <QDialog>
#include <QSharedPointer>
#include "Actions/action.h"

#include "Actions/openappaction.h"
#include "Actions/openurlaction.h"
#include "Actions/typekeystrokeaction.h"
#include "Actions/openvscodefolderaction.h"

namespace Ui {
class AddActionDialog;
}

class AddActionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddActionDialog(QWidget *parent = nullptr);
    ~AddActionDialog();

    QSharedPointer<Action> getAction() const;
    void setAction(QSharedPointer<Action> actionToEdit);

private slots:
    void on_browseAppButton_clicked();
    void on_browseVSCodeFolderButton_clicked();
    void on_browseFolderButton_clicked();

private:
    Ui::AddActionDialog *ui;
    QSharedPointer<Action> editingAction = nullptr;
};

#endif // ADDACTIONDIALOG_H
