#ifndef RUNCOMMANDACTION_H
#define RUNCOMMANDACTION_H

#include "action.h"
#include <QString>

class RunCommandAction : public Action
{
public:
    explicit RunCommandAction(const QString& command);
    void execute() override;
    QString description() const override;
    QJsonObject toJson() const override;
    QString getCommand() const { return command; }
    void setCommand(const QString& comm) { command = comm; }
private:
    QString command;
};
#endif // RUNCOMMANDACTION_H
