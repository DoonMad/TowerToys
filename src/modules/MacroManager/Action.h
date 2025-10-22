#ifndef ACTION_H
#define ACTION_H

#include <QString>
#include <QJsonObject>

class Action {
public:
    virtual ~Action() {}
    virtual void execute() = 0;
    virtual QString description() const = 0;
    virtual QJsonObject toJson() const = 0;
};

#endif // ACTION_H
