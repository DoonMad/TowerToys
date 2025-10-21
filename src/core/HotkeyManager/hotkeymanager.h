#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <QObject>
#include <QHash>
#include "MacroManager/macro.h"
#include "qhotkey.h"


class HotkeyManager : public QObject
{
    Q_OBJECT
public:
    explicit HotkeyManager(QObject *parent = nullptr);
    ~HotkeyManager();

private:
    QHash<QSharedPointer<Macro>, QHotkey*> activeHotkeys;
    QHash<QKeySequence, QSharedPointer<Macro>> activeSequences;

public slots:
    void registerMacro(QSharedPointer<Macro> macro);
    void unregisterMacro(QSharedPointer<Macro> macro);
    void reregisterMacro(QSharedPointer<Macro> macro);

private slots:
    void onHotkeyActivated();

signals:
    void hotkeyStatusChanged(QSharedPointer<Macro> macro, bool success, QString message);
};

#endif // HOTKEYMANAGER_H
