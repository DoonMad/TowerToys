#include "openappaction.h"
#include <QProcess>

void OpenAppAction::execute() {
    QProcess::startDetached(appPath);
}
