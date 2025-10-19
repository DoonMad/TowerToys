#ifndef ADDACTIONDIALOG_H
#define ADDACTIONDIALOG_H

#include <QDialog>

namespace Ui {
class AddActionDialog;
}

class AddActionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddActionDialog(QWidget *parent = nullptr);
    ~AddActionDialog();

private:
    Ui::AddActionDialog *ui;
};

#endif // ADDACTIONDIALOG_H
