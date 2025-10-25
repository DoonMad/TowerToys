#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

private slots:
    // Slot for when the combo box selection changes
    void onStyleComboBoxCurrentIndexChanged(int index);
    void onStartupCheckBoxToggled(bool checked);

private:
    void loadSettings();
    void saveSettings(const QString &styleName);
    void setStartupRegistry(bool enabled);

    Ui::SettingsWidget *ui;
};

#endif // SETTINGSWIDGET_H
