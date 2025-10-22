#include "MacroManager/macro.h"
#include "Actions/action.h"
#include <QJsonArray>

Macro::Macro(QObject *parent)
    : QObject{parent}
{}

void Macro::addAction(QSharedPointer<Action> action) {
    actions.append(action);
}

void Macro::execute() {
    for (auto &a : actions)
        a->execute();

    emit executed();
}

void Macro::removeAction(QSharedPointer<Action> action)
{
    actions.removeOne(action);
}

QJsonObject Macro::toJson() const
{
    QJsonObject macroObj;
    macroObj["name"] = name;
    macroObj["shortcut"] = shortcut;

    // Create a JSON array to hold all the actions
    QJsonArray actionsArray;
    for (const QSharedPointer<Action>& action : actions) {
        actionsArray.append(action->toJson());
    }

    macroObj["actions"] = actionsArray;
    return macroObj;
}
