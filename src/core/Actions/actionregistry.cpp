#include "actionregistry.h"
#include "openappaction.h"
#include "openurlaction.h"
#include "runcommandaction.h"
#include "delayaction.h"
#include "openfolderaction.h"
#include "openvscodefolderaction.h"
#include "typekeystrokeaction.h"

ActionRegistry* ActionRegistry::s_instance = nullptr;

ActionRegistry* ActionRegistry::instance()
{
    if (!s_instance) {
        s_instance = new ActionRegistry();
    }
    return s_instance;
}

void ActionRegistry::registerDefaultActions()
{
    ActionRegistry* reg = ActionRegistry::instance();

    reg->registerAction("Open App", 
                        "Launch an application (.exe) or file. Use full absolute paths.",
                        "// For 'Open App', provide 'path' (absolute path to executable or file). Example: 'C:\\\\Program Files\\\\Google\\\\Chrome\\\\Application\\\\chrome.exe'",
                        [](const QJsonObject& obj) { return QSharedPointer<OpenAppAction>::create(obj["path"].toString()); });

    reg->registerAction("Open URL", 
                        "Open a website URL in the default browser.",
                        "// For 'Open URL', provide 'url' (e.g. 'https://youtube.com' or 'https://codeforces.com/contests').",
                        [](const QJsonObject& obj) { return QSharedPointer<OpenURLAction>::create(obj["url"].toString()); });

    reg->registerAction("Run Command", 
                        "Run a command in CMD/Powershell.",
                        "// For 'Run Command', provide 'command' (cmd/powershell command).",
                        [](const QJsonObject& obj) { return QSharedPointer<RunCommandAction>::create(obj["command"].toString()); });

    reg->registerAction("Delay", 
                        "Pause macro execution for a specified number of milliseconds.",
                        "// For 'Delay', provide 'delayMs' (integer milliseconds, e.g. 500).",
                        [](const QJsonObject& obj) { return QSharedPointer<DelayAction>::create(obj["delayMs"].toInt(500)); });

    reg->registerAction("Open Folder", 
                        "Open a folder directory in Windows Explorer.",
                        "// For 'Open Folder', provide 'path' (absolute directory path).",
                        [](const QJsonObject& obj) { return QSharedPointer<OpenFolderAction>::create(obj["path"].toString()); });

    reg->registerAction("Open Folder in VS Code", 
                        "Open a directory directly in Visual Studio Code.",
                        "// For 'Open Folder in VS Code', provide 'path' (absolute directory path).",
                        [](const QJsonObject& obj) { return QSharedPointer<OpenVSCodeFolderAction>::create(obj["path"].toString()); });

    reg->registerAction("Type Keystroke", 
                        "Simulate typing text keystrokes.",
                        "// For 'Type Keystroke', provide 'text' (string to type).",
                        [](const QJsonObject& obj) { return QSharedPointer<TypeKeystrokeAction>::create(obj["text"].toString()); });
}

ActionRegistry::ActionRegistry()
{
}

ActionRegistry::~ActionRegistry()
{
}

void ActionRegistry::registerAction(const QString& typeName, 
                                    const QString& description,
                                    const QString& jsonSchema,
                                    std::function<QSharedPointer<Action>(const QJsonObject&)> factory)
{
    ActionInfo info;
    info.name = typeName;
    info.description = description;
    info.jsonSchema = jsonSchema;
    info.factory = factory;
    m_actions.insert(typeName, info);
}

QSharedPointer<Action> ActionRegistry::createAction(const QString& typeName, const QJsonObject& jsonObj) const
{
    if (m_actions.contains(typeName)) {
        return m_actions.value(typeName).factory(jsonObj);
    }
    return nullptr;
}

QMap<QString, ActionInfo> ActionRegistry::getRegisteredActions() const
{
    return m_actions;
}
