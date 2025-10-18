#ifndef OPENAPPACTION_H
#define OPENAPPACTION_H

#include "action.h"

class OpenAppAction : public Action
{
    QString appPath;
public:
    OpenAppAction(const QString &path) : appPath(path) {};
    void execute() override;
    QString description() const override{
        return "Open "+appPath;
    }
};

#endif // OPENAPPACTION_H
