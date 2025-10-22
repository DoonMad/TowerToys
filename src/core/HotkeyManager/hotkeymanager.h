#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <QObject>
#include <QMap>
#include "MacroManager/macro.h"
#include "qhotkey.h"
#include <QSet>


class HotkeyManager : public QObject
{
    Q_OBJECT
public:
    explicit HotkeyManager(QObject *parent = nullptr);
    ~HotkeyManager();

private:
    QMap<QSharedPointer<Macro>, QHotkey*> activeHotkeys;
    QMap<QKeySequence, QSharedPointer<Macro>> activeSequences;
    QSet<QKeySequence> blacklist;

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
