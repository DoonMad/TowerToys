#ifndef MACROMANAGER_H
#define MACROMANAGER_H

#include <QObject>
#include <QList>
#include "MacroManager/macro.h"

class MacroManager : public QObject
{
    Q_OBJECT
public:
    explicit MacroManager(QObject *parent = nullptr);
    QList<QSharedPointer<Macro>> getMacros() const { return macros; }

signals:
    void macroAdded(QSharedPointer<Macro> macro);
    void macroRemoved(QSharedPointer<Macro> macro);
    void macroEdited(QSharedPointer<Macro> macro);
    void hotkeyStatus(QString macroName, bool success, QString message);

public slots:
    void addMacro(QSharedPointer<Macro> macro);
    void removeMacro(QSharedPointer<Macro> macro);
    void loadMacros();

private slots:
    void saveMacros() const;

private:
    QList<QSharedPointer<Macro>> macros;
    QString getSavePath() const;
};

#endif // MACROMANAGER_H
