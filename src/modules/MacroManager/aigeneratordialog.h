#ifndef AIGENERATORDIALOG_H
#define AIGENERATORDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class AIGeneratorDialog;
}

class AIGeneratorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AIGeneratorDialog(QWidget *parent = nullptr);
    ~AIGeneratorDialog();

    QString getPrompt() const;
    void setStatus(const QString &text);
    void setGenerating(bool generating);

private slots:
    void onGenerateClicked();

private:
    Ui::AIGeneratorDialog *ui;
};

#endif // AIGENERATORDIALOG_H
