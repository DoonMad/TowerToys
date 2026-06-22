#ifndef ACTIONREGISTRY_H
#define ACTIONREGISTRY_H

#include <QString>
#include <QJsonObject>
#include <QSharedPointer>
#include <QMap>
#include <functional>
#include "action.h"

// Metadata describing an action type for the AI
struct ActionInfo {
    QString name;
    QString description;
    QString jsonSchema; // Example: "// For 'Open App', provide 'path' (executable path)"
    std::function<QSharedPointer<Action>(const QJsonObject&)> factory;
};

class ActionRegistry
{
public:
    static ActionRegistry* instance();
    static void registerDefaultActions();

    void registerAction(const QString& typeName, 
                        const QString& description,
                        const QString& jsonSchema,
                        std::function<QSharedPointer<Action>(const QJsonObject&)> factory);

    QSharedPointer<Action> createAction(const QString& typeName, const QJsonObject& jsonObj) const;
    
    QMap<QString, ActionInfo> getRegisteredActions() const;

private:
    ActionRegistry();
    ~ActionRegistry();

    static ActionRegistry* s_instance;
    QMap<QString, ActionInfo> m_actions;
};

#endif // ACTIONREGISTRY_H
