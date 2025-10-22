#include "typekeystrokeaction.h"
#include <QDebug>

TypeKeystrokeAction::TypeKeystrokeAction(const QString& text) : text(text) {}

void TypeKeystrokeAction::execute() {
    // Platform-specific implementation is complex.
    // For now, just logging it.
    qDebug() << "SIMULATE KEYSTROKE:" << text;
}

QString TypeKeystrokeAction::description() const {
    return "Type: " + text;
}


QJsonObject TypeKeystrokeAction::toJson() const
{
    QJsonObject obj;
    obj["type"] = "TypeKeystrokeAction";
    obj["text"] = text;
    return obj;
}
