#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSharedPointer>
#include "MacroManager/macro.h"

class AIManager : public QObject
{
    Q_OBJECT
public:
    explicit AIManager(QObject *parent = nullptr);

    // Returns true if generation started, false if missing API key
    bool generateMacro(const QString &prompt);

signals:
    void macroGenerated(QSharedPointer<Macro> macro);
    void generationFailed(const QString &errorMsg);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager;
};

#endif // AIMANAGER_H
