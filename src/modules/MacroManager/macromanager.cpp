#include "macromanager.h"
#include "QDebug"
#include "macro.h"

MacroManager::MacroManager(QObject *parent)
    : QObject{parent}
{

}

void MacroManager::addMacro(QSharedPointer<Macro> macro){
    // qDebug()<<"Adding macro..."<<macro->name;

    macros.append(macro);
    emit macroAdded(macro);
}

void MacroManager::removeMacro(QSharedPointer<Macro> macro)
{
    if (macros.removeOne(macro)) {
        // Only emit if removal was successful
        emit macroRemoved(macro);
    }
}
