#include "settingswidget.h"
#include "ui_settingswidget.h"

#include <QStyleFactory> // To get available styles
#include <QApplication>  // To apply the style
#include <QSettings>     // To save the user's choice
#include <QDebug>
#include <QDir>
#include <QCheckBox>

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    QStringList styleKeys = QStyleFactory::keys();
    ui->styleComboBox->addItems(styleKeys);

    loadSettings();

    connect(ui->styleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsWidget::onStyleComboBoxCurrentIndexChanged);
    connect(ui->startupCheckBox, &QCheckBox::toggled, this, &SettingsWidget::onStartupCheckBoxToggled);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::onStartupCheckBoxToggled(bool checked)
{
    setStartupRegistry(checked); // Call helper to modify registry

    QSettings settings; // Uses org/app name set in main.cpp
    settings.setValue("startOnStartup", checked);
    qDebug() << "Saved startOnStartup setting:" << checked;
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

    bool startOnStartup = settings.value("startOnStartup", true).toBool(); // Default to false
    ui->startupCheckBox->setChecked(startOnStartup);
    qDebug() << "Loaded startOnStartup setting:" << startOnStartup;
}

void SettingsWidget::saveSettings(const QString &styleName)
{
    QSettings settings;
    settings.setValue("applicationStyle", styleName);
    qDebug() << "Saved style setting:" << styleName;
}

void SettingsWidget::setStartupRegistry(bool enabled)
{
    // Use QSettings to access the specific registry key
    QSettings registrySettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    QString appName = QApplication::applicationName(); // "TowerToys"
    QString appPath = QDir::toNativeSeparators(QApplication::applicationFilePath()); // Full path to .exe

    if (enabled) {
        // Add entry: Key = AppName, Value = "C:\Path\To\YourApp.exe"
        registrySettings.setValue(appName, appPath);
        qDebug() << "Added to startup:" << appPath;
    } else {
        // Remove entry by key name
        registrySettings.remove(appName);
        qDebug() << "Removed from startup:" << appName;
    }
}
