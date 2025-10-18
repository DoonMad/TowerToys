#include "MacroManager/macro.h"
#include "Actions/action.h"

Macro::Macro(QObject *parent)
    : QObject{parent}
{}

void Macro::addAction(QSharedPointer<Action> action) {
    actions.append(action);
}

void Macro::execute() {
    for (auto &a : actions)
        a->execute();
}
