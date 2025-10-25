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
    QString getText() const { return text; }
    void setText(const QString& txt) { this->text = txt; qDebug() << "keystroke updated: " << this->text; }

private:
    QString text;
};

#endif // TYPEKEYSTROKEACTION_H
