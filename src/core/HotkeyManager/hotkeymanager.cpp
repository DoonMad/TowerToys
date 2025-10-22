#include "hotkeymanager.h"
#include <QKeySequence>
#include <QDebug>
#include "MacroManager/macro.h"

HotkeyManager::HotkeyManager(QObject *parent)
    : QObject{parent}
{
    blacklist.insert(QKeySequence::fromString("Ctrl+C"));
    blacklist.insert(QKeySequence::fromString("Ctrl+V"));
    blacklist.insert(QKeySequence::fromString("Ctrl+X"));
    blacklist.insert(QKeySequence::fromString("Ctrl+A"));
    blacklist.insert(QKeySequence::fromString("Ctrl+Z"));
    blacklist.insert(QKeySequence::fromString("Alt+F4"));
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

    if (blacklist.contains(sequence)) {
        emit hotkeyStatusChanged(macro, false, "Shortcut is reserved by the system.");
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
    activeSequences.insert(sequence, macro);

    emit hotkeyStatusChanged(macro, true, "Registered!");
    qDebug() << "Registered hotkey:" << sequence.toString();
}

void HotkeyManager::unregisterMacro(QSharedPointer<Macro> macro)
{
    if (!macro) {
        return;
    }

    if (!activeHotkeys.contains(macro)) {
        qWarning() << "Tried to unregister a macro that was not in activeHotkeys:" << macro->name;
        return;
    }

    QHotkey* hotkey = activeHotkeys.value(macro);
    QKeySequence sequence = QKeySequence::fromString(macro->shortcut);

    if (!activeSequences.contains(sequence)) {
        return;
    }

    activeHotkeys.remove(macro);
    activeSequences.remove(sequence);
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

