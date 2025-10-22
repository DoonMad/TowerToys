#ifndef MACRO_H
#define MACRO_H

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include "action.h"
#include <QJsonObject>

class Macro : public QObject
{
    Q_OBJECT
public:
    QString name, shortcut, appContext; // Eg: "chrome.exe", "vscode.exe", "" for universal
    QList<QSharedPointer<Action>> actions;

    explicit Macro(QObject *parent = nullptr);

    void addAction(QSharedPointer<Action> action);
    void removeAction(QSharedPointer<Action> action);
    void execute();
    QJsonObject toJson() const;

signals:
    void executed();
    void failed(QString reason);
};

#endif // MACRO_H
