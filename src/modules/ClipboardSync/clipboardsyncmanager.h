#ifndef CLIPBOARDSYNCMANAGER_H
#define CLIPBOARDSYNCMANAGER_H

#include <QObject>
#include <QHttpServerResponder>

struct QHttpServerRequest;

class ClipboardSyncManager : public QObject
{
    Q_OBJECT
public:
    explicit ClipboardSyncManager(QObject *parent = nullptr);

public slots:
    /**
     * @brief Slot connected to AppController. Updates the cached clipboard text.
     */
    void updateClipboardText(const QString &text);

    /**
     * @brief Slot connected to LocalShareServer::clipboardDataRequest.
     * Responds to web clients asking for the current clipboard.
     */
    void handleClipboardDataRequest(const QHttpServerRequest &request, QHttpServerResponder &responder);

private:
    QString currentClipboardText;

signals:
};

#endif // CLIPBOARDSYNCMANAGER_H
