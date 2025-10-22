#include "modulemanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

ModuleManager::ModuleManager(QObject *parent) : QObject(parent) {}

QMap<QString, ModuleInfo> ModuleManager::loadModules() {
    QMap<QString, ModuleInfo> modules;

    const QString jsonPath = ":/resources/modules.json";
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly))
        return modules;

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    // Adding validation
    if (doc.isNull()) {
        qWarning() << "Failed to parse modules.json:" << parseError.errorString();
        return modules;
    }

    if (!doc.isObject()) {
        qWarning() << "modules.json does not contain a valid JSON object.";
        return modules;
    }

    QJsonArray arr = doc.object()["modules"].toArray();

    for (const QJsonValue &val : arr) {
        QJsonObject obj = val.toObject();
        ModuleInfo info;

        info.name = obj["name"].toString();
        info.description = obj["description"].toString();
        info.iconPath = obj["icon"].toString();
        info.widgetClass = obj["widget"].toString();

        if (!info.name.isEmpty()) {
            modules[info.name] = info;
        }
    }

    qDebug() << "Successfully loaded" << modules.count() << "modules.";
    return modules;
}
