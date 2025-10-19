#ifndef ADDACTIONDIALOG_H
#define ADDACTIONDIALOG_H

#include <QDialog>
#include <QSharedPointer>
#include "Actions/action.h"

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

private slots:
    void on_browseAppButton_clicked();

private:
    Ui::AddActionDialog *ui;
};

#endif // ADDACTIONDIALOG_H
