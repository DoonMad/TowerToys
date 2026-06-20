#include "typekeystrokeaction.h"
#include <QDebug>

TypeKeystrokeAction::TypeKeystrokeAction(const QString& text) : text(text) {}

#ifdef Q_OS_WIN
#include <windows.h>
#endif

void TypeKeystrokeAction::execute() {
    qDebug() << "SIMULATE KEYSTROKE:" << text;

#ifdef Q_OS_WIN
    for (int i = 0; i < text.length(); ++i) {
        ushort unicodeChar = text.at(i).unicode();

        INPUT inputs[2] = {};

        // Key down
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wScan = unicodeChar;
        inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;

        // Key up
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wScan = unicodeChar;
        inputs[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

        SendInput(2, inputs, sizeof(INPUT));
    }
#else
    qWarning() << "TypeKeystrokeAction is only implemented for Windows.";
#endif
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
