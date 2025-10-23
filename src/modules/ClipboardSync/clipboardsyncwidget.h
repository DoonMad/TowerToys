#ifndef CLIPBOARDSYNCWIDGET_H
#define CLIPBOARDSYNCWIDGET_H

#include <QWidget>
#include "moduleinfo.h"

class ClipboardSyncManager;
class AppController;

namespace Ui {
class ClipboardSyncWidget;
}

class ClipboardSyncWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClipboardSyncWidget(AppController* controller, // <-- ADD controller
                                 ClipboardSyncManager* manager,
                                 const ModuleInfo& info,
                                 QWidget *parent = nullptr);
    ~ClipboardSyncWidget();

private:
    Ui::ClipboardSyncWidget *ui;
    ClipboardSyncManager* manager;
    AppController* controller;

private slots:
    void on_enableClipboardSyncCheckbox_toggled(bool checked);
};

#endif // CLIPBOARDSYNCWIDGET_H
