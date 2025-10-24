#include "filesharewidget.h"
#include "ui_filesharewidget.h"

#include "LocalShareServer/localshareserver.h"
#include "FileShare/filesharemanager.h"
#include "qrcodegen.hpp"
#include <QPainter>

using qrcodegen::QrCode;

FileShareWidget::FileShareWidget(LocalShareServer* server,
                                 FileShareManager* manager,
                                 const ModuleInfo& info,
                                 QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileShareWidget),
    server(server),
    manager(manager)
{
    ui->setupUi(this);

    // ui->titleLabel->setText(info.name);
    // ui->descriptionLabel->setText(info.description);

    // Connect the server's status signal to UI slot
    connect(server, &LocalShareServer::serverStatusChanged,
            this, &FileShareWidget::onServerStatusChanged);

    // Connect the manager's notification signal to UI slot
    connect(manager, &FileShareManager::newNotification,
            this, &FileShareWidget::onNewNotification);

    // Set initial state
    // ui->serverInfoGroup->setVisible(false);
}

FileShareWidget::~FileShareWidget()
{
    delete ui;
}

// This slot is auto-connected to the "toggleServerButton"
void FileShareWidget::on_toggleServerButton_toggled(bool checked)
{
    if (checked) {
        // Button is "on"
        server->startServer();
    } else {
        // Button is "off"
        server->stopServer();
    }
}

void FileShareWidget::onServerStatusChanged(bool running, const QString &serverUrl)
{
    if (running) {
        ui->toggleServerButton->setText("Stop Server");
        ui->urlLabel->setText(QString("Connect at: %1").arg(serverUrl));
        // ui->serverInfoGroup->setVisible(true);
        generateQrCode(serverUrl);
    } else {
        ui->toggleServerButton->setChecked(false); // Ensure button is "off"
        ui->toggleServerButton->setText("Start Server");
        ui->urlLabel->setText("Status: Server is OFF");
        // ui->serverInfoGroup->setVisible(false);
        ui->qrCodeLabel->setPixmap(QPixmap()); // Clear the QR code
        ui->qrCodeLabel->setText("QR Code");
        ui->qrCodeLabel->setStyleSheet("background-color: #eee;");
    }
}

void FileShareWidget::onNewNotification(const QString &message)
{
    // Add the new message to the top of the list
    ui->notificationList->insertItem(0, message);
}

// --- QR Code Generation Logic ---
void FileShareWidget::generateQrCode(const QString &text)
{
    if (text.isEmpty()) {
        return;
    }

    // 1. Create the QR Code object from the text
    QrCode qr = QrCode::encodeText(text.toUtf8().constData(), QrCode::Ecc::MEDIUM);

    // 2. Create a QImage to draw on
    const int scale = 4; // Scale factor for the image
    const int border = 2; // Border in "modules" (QR code squares)
    QImage image( (qr.getSize() + 2 * border) * scale,
                 (qr.getSize() + 2 * border) * scale,
                 QImage::Format_RGB32);
    image.fill(Qt::white); // White background

    // 3. Create a QPainter to draw the QR code
    QPainter painter(&image);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);

    // 4. Draw the QR code "modules" (squares)
    for (int y = 0; y < qr.getSize(); ++y) {
        for (int x = 0; x < qr.getSize(); ++x) { // <-- CORRECTED THIS LINE
            if (qr.getModule(x, y)) {
                // (x + border) offsets for the border
                painter.drawRect((x + border) * scale, (y + border) * scale, scale, scale);
            }
        }
    }

    // 5. Display the QImage in our QLabel
    ui->qrCodeLabel->setPixmap(QPixmap::fromImage(image));
}


