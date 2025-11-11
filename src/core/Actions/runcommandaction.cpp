#include "runcommandaction.h"
#include <QProcess>

RunCommandAction::RunCommandAction(const QString& command) : command(command) {}

void RunCommandAction::execute()
{
    if (command.isEmpty()) {
        return;
    }

    qDebug() << "Attempting to execute command:" << command;

    // Split the command string by spaces.
    // NOTE: This is a simple split and won't handle file paths with spaces correctly.
    // For v1, this is okay. A robust solution would need more complex parsing.
    QStringList parts = command.split(' ');

    if (parts.isEmpty()) {
        return;
    }

    // The first part is the program to run.
    QString program = parts.takeFirst(); // e.g., "cmd" or "ipconfig"

    // The rest are the arguments.
    QStringList arguments = parts; // e.g., ["/k", "ipconfig"] or []

    // On Windows, if the user types "cmd", they mean "cmd.exe"
    if (program.toLower() == "cmd") {
        program = "cmd.exe";
    }

    // Try to run the program with its arguments.
    // This overload correctly handles separating the program from args.
    bool success = QProcess::startDetached(program, arguments);

    if (success) {
        qDebug() << "Successfully started" << program << "with args" << arguments;
    } else {
        qWarning() << "Failed to start" << program << "with args" << arguments;
        qWarning() << "Attempting fallback: running the whole string (might fail).";

        // Fallback for simple cases like "notepad.exe"
        // This might still fail for "ipconfig" (runs in terminal)
        // or "cmd /k ..." (parser error)
        QProcess::startDetached(command);
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
