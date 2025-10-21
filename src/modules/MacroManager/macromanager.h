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

signals:
    void macroAdded(QSharedPointer<Macro> macro);
    void macroRemoved(QSharedPointer<Macro> macro);
    void macroEdited(QSharedPointer<Macro> macro);

public slots:
    void addMacro(QSharedPointer<Macro> macro);
    void removeMacro(QSharedPointer<Macro> macro);

private:
    QList<QSharedPointer<Macro>> macros;
};

#endif // MACROMANAGER_H
