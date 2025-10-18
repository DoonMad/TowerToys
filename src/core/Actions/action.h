#ifndef ACTION_H
#define ACTION_H

#include <QString>

class Action{
public:
    virtual ~Action(){}
    virtual void execute() = 0;
    virtual QString description() const = 0;
};

#endif // ACTION_H
