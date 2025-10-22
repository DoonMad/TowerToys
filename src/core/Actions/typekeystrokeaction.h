#ifndef TYPEKEYSTROKEACTION_H
#define TYPEKEYSTROKEACTION_H

#include "action.h"
#include <QString>

class TypeKeystrokeAction : public Action
{
public:
    TypeKeystrokeAction(const QString& text);
    void execute() override;
    QString description() const override;
    QJsonObject toJson() const override;

private:
    QString text;
};

#endif // TYPEKEYSTROKEACTION_H
