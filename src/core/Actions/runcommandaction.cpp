#include "runcommandaction.h"
#include <QProcess>

RunCommandAction::RunCommandAction(const QString& command) : command(command) {}

void RunCommandAction::execute()
{
    if (command.isEmpty()) {
        return;
    }

    qDebug() << "Attempting to execute command in new console:" << command;

    // Use cmd.exe to launch a new console window
    // /c : execute string and terminate
    // start : open a new window
    // cmd.exe /k : open command prompt and keep it open after running the command
    // The final structure is: cmd.exe /c start cmd.exe /k "user_command"
    
    QString program = "cmd.exe";
    QStringList arguments;
    arguments << "/c" << "start" << "cmd.exe" << "/k" << command;

    bool success = QProcess::startDetached(program, arguments);

    if (success) {
        qDebug() << "Successfully launched console for command:" << command;
    } else {
        qWarning() << "Failed to launch console for command:" << command;
    }
}


QString RunCommandAction::description() const
{
    return "Run Command: " + command;
}

QJsonObject RunCommandAction::toJson() const
{
    QJsonObject obj;
    obj["type"] = "RunCommandAction";
    obj["command"] = command;
    return obj;
}
