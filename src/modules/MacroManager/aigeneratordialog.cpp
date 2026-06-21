#include "aigeneratordialog.h"
#include "ui_aigeneratordialog.h"

AIGeneratorDialog::AIGeneratorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AIGeneratorDialog)
{
    ui->setupUi(this);

    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->generateButton, &QPushButton::clicked, this, &AIGeneratorDialog::onGenerateClicked);
}

AIGeneratorDialog::~AIGeneratorDialog()
{
    delete ui;
}

QString AIGeneratorDialog::getPrompt() const
{
    return ui->promptTextEdit->toPlainText();
}

void AIGeneratorDialog::setStatus(const QString &text)
{
    ui->statusLabel->setText(text);
}

void AIGeneratorDialog::setGenerating(bool generating)
{
    ui->generateButton->setEnabled(!generating);
    ui->promptTextEdit->setEnabled(!generating);
    if (generating) {
        ui->generateButton->setText("Generating...");
    } else {
        ui->generateButton->setText("Generate");
    }
}

void AIGeneratorDialog::onGenerateClicked()
{
    if (getPrompt().trimmed().isEmpty()) return;
    accept(); // Closes dialog and returns QDialog::Accepted
}
