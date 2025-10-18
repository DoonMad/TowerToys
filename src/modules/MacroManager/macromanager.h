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

    void addMacro(const Macro &macro);
    void triggerShortcut(const QString &shortcut, const QString &activeApp);

signals:
    void macroAdded(const QString &name);
    void macroExecuted(const QString &name);

private:
    QList<Macro> macros;
};

#endif // MACROMANAGER_H
