#ifndef MACRO_H
#define MACRO_H

#include <QObject>
#include <QString>
#include <QSharedDataPointer>
#include "action.h"

class Macro : public QObject
{
    Q_OBJECT
public:
    QString name;
    QString shortcut;
    QString appContext; // Eg: "chrome.exe", "vscode.exe", "" for universal
    QList<QSharedPointer<Action>> actions;

    explicit Macro(QObject *parent = nullptr);

    void addAction(QSharedPointer<Action> action);
    void execute();

signals:
    void executed();
    void failed(QString reason);
};

#endif // MACRO_H
