#include "macromanager.h"
#include "QDebug"
#include "macro.h"

MacroManager::MacroManager(QObject *parent)
    : QObject{parent}
{

}

void MacroManager::addMacro(const Macro &macro){
    qDebug()<<"Adding macro..."<<macro.name;

    macros.append(macro);
    emit macroAdded(macro.name);
}

void MacroManager::triggerShortcut(const QString &shortcut, const QString &activeApp) {
    for (auto &m : macros) {
        if (m.shortcut == shortcut &&
            (m.appContext.isEmpty() || m.appContext == activeApp)) {
            qDebug() << "Executing macro:" << m.name;
            m.execute();
            emit macroExecuted(m.name);
            return;
        }
    }
}
