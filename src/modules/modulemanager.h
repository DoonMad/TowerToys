#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include "moduleinfo.h"

class ModuleManager : public QObject
{
    Q_OBJECT
public:
    explicit ModuleManager(QObject *parent = nullptr);
    QMap<QString, ModuleInfo> loadModules();

signals:
};

#endif // MODULEMANAGER_H
