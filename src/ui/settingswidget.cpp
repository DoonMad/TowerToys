#include "settingswidget.h"
#include "ui_settingswidget.h"

#include <QStyleFactory> // To get available styles
#include <QApplication>  // To apply the style
#include <QSettings>     // To save the user's choice
#include <QDebug>

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    // 1. Populate the combo box with available styles
    QStringList styleKeys = QStyleFactory::keys();
    ui->styleComboBox->addItems(styleKeys);

    // 2. Load the previously saved setting (if any)
    loadSettings();

    // 3. Connect the combo box signal to our slot
    connect(ui->styleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsWidget::onStyleComboBoxCurrentIndexChanged);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::onStyleComboBoxCurrentIndexChanged(int index)
{
    if (index < 0) return;

    QString selectedStyle = ui->styleComboBox->currentText();

    // Apply the selected style to the entire application
    QStyle *style = QStyleFactory::create(selectedStyle);
    if (style) {
        QApplication::setStyle(style);
        qDebug() << "Applied style:" << selectedStyle;
        // Save the setting
        saveSettings(selectedStyle);
    } else {
        qWarning() << "Failed to create style:" << selectedStyle;
    }
}

void SettingsWidget::loadSettings()
{
    QSettings settings;
    QString savedStyle = settings.value("applicationStyle", QApplication::style()->objectName()).toString();

    // Find the saved style in the combo box and select it
    int index = ui->styleComboBox->findText(savedStyle);
    if (index != -1) {
        ui->styleComboBox->setCurrentIndex(index);
        qDebug() << "Loaded style setting:" << savedStyle;
    } else {
        qDebug() << "Saved style not found, using default:" << QApplication::style()->objectName();
    }
}

void SettingsWidget::saveSettings(const QString &styleName)
{
    QSettings settings;
    settings.setValue("applicationStyle", styleName);
    qDebug() << "Saved style setting:" << styleName;
}
