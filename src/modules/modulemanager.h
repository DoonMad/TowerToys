#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <QObject>
#include <QHash>
#include <QString>
#include "moduleinfo.h"

class ModuleManager : public QObject
{
    Q_OBJECT
public:
    explicit ModuleManager(QObject *parent = nullptr);
    QHash<QString, ModuleInfo> loadModules();

signals:
};

#endif // MODULEMANAGER_H
