#ifndef FILESHAREWIDGET_H
#define FILESHAREWIDGET_H

#include <QWidget>
#include <QPixmap>
#include "moduleinfo.h"

class LocalShareServer;
class FileShareManager;

namespace Ui {
class FileShareWidget;
}

class FileShareWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileShareWidget(LocalShareServer* server,
                             FileShareManager* manager,
                             const ModuleInfo& info,
                             QWidget *parent = nullptr);
    ~FileShareWidget();

private:
    /**
     * @brief Generates a QR code from a string and displays it.
     */
    void generateQrCode(const QString &text);

    Ui::FileShareWidget *ui;
    LocalShareServer* server;
    FileShareManager* manager;

private slots:
    // Slot for the Start/Stop button
    void on_toggleServerButton_toggled(bool checked);

    // Slot for server status signals
    void onServerStatusChanged(bool running, const QString &serverUrl);

    // Slot for file upload notifications
    void onNewNotification(const QString &message);
};

#endif // FILESHAREWIDGET_H
