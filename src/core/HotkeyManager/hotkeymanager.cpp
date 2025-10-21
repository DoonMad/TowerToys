#include "hotkeymanager.h"
#include <QKeySequence>
#include <QDebug>
#include "MacroManager/macro.h"

HotkeyManager::HotkeyManager(QObject *parent)
    : QObject{parent}
{

}

HotkeyManager::~HotkeyManager()
{
    qDeleteAll(activeHotkeys);
}

void HotkeyManager::registerMacro(QSharedPointer<Macro> macro)
{
    if (!macro || macro->shortcut.isEmpty()) {
        return;
    }

    QKeySequence sequence = QKeySequence::fromString(macro->shortcut);
    if (sequence.isEmpty()) {
        emit hotkeyStatusChanged(macro, false, "Invalid hotkey format.");
        return;
    }

    if (activeSequences.contains(sequence)) {
        emit hotkeyStatusChanged(macro, false, "In use by another macro.");
        return;
    }

    // The 'true' argument automatically registers it with the OS
    QHotkey* hotkey = new QHotkey(sequence, true, this);

    if (!hotkey->isRegistered()) {
        delete hotkey;
        emit hotkeyStatusChanged(macro, false, "In use by another app.");
        return;
    }

    // Connecting hotkey's activated() signal to central handler slot
    connect(hotkey, &QHotkey::activated, this, &HotkeyManager::onHotkeyActivated);
    activeHotkeys.insert(macro, hotkey);

    emit hotkeyStatusChanged(macro, true, "Registered!");
    qDebug() << "Registered hotkey:" << sequence.toString();
}

void HotkeyManager::unregisterMacro(QSharedPointer<Macro> macro)
{
    if (!macro) {
        return;
    }

    QKeySequence sequence = QKeySequence::fromString(macro->shortcut);
    if (!activeSequences.contains(sequence)) {
        return;
    }

    QSharedPointer<Macro> storedMacro = activeSequences.value(sequence);

    if (!activeHotkeys.contains(macro)) return;
    QHotkey* hotkey = activeHotkeys.value(macro);
    activeHotkeys.remove(macro);
    delete hotkey;

    qDebug() << "Unregistered hotkey for macro:" << macro->name;
}

void HotkeyManager::reregisterMacro(QSharedPointer<Macro> macro)
{
    unregisterMacro(macro);
    registerMacro(macro);
}

void HotkeyManager::onHotkeyActivated()
{
    QHotkey* hotkey = qobject_cast<QHotkey*>(sender());
    if (!hotkey) {
        return;
    }
    // We are looking for the 'key' (Macro) whose 'value' (QHotkey*) matches.
    const QSharedPointer<Macro>& macro = activeHotkeys.key(hotkey);
    if (macro) {
        qDebug() << "Hotkey activated, executing macro:" << macro->name;
        macro->execute();
    }
}

