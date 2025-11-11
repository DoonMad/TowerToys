#include "macromanager.h"
#include "QDebug"
#include "macro.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include "Actions/openappaction.h"
#include "Actions/openurlaction.h"
#include "Actions/typekeystrokeaction.h"
#include "Actions/openvscodefolderaction.h"
#include "Actions/openfolderaction.h"
#include "Actions/runcommandaction.h"

MacroManager::MacroManager(QObject *parent)
    : QObject{parent}
{
    connect(this, &MacroManager::macroAdded, this, &MacroManager::saveMacros);
    connect(this, &MacroManager::macroRemoved, this, &MacroManager::saveMacros);
    connect(this, &MacroManager::macroEdited, this, &MacroManager::saveMacros);

    loadMacros();
}

void MacroManager::addMacro(QSharedPointer<Macro> macro){
    // qDebug()<<"Adding macro..."<<macro->name;

    macros.append(macro);
    emit macroAdded(macro);
}

void MacroManager::removeMacro(QSharedPointer<Macro> macro)
{
    if (macros.removeOne(macro)) {
        // Only emit if removal was successful
        emit macroRemoved(macro);
    }
}

QString MacroManager::getSavePath() const
{
    // Finding a standard, writable location for app data
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // On Windows, this is C:/Users/<User>/AppData/Roaming/TowerToys
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath("."); // Createing a folder if it doesn't exist
    }

    return path + "/macros.json";
}


void MacroManager::saveMacros() const
{
    // Creating a JSON array to hold all macro objects
    QJsonArray macrosArray;
    for (const QSharedPointer<Macro>& macro : macros) {
        macrosArray.append(macro->toJson());
    }

    // Wrapping the array in a root object
    QJsonObject rootObj;
    rootObj["macros"] = macrosArray;

    QFile saveFile(getSavePath());
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't open macros.json for writing:" << getSavePath();
        return;
    }

    // Writing the JSON document to the file
    saveFile.write(QJsonDocument(rootObj).toJson());
    saveFile.close();

    qDebug() << "Macros saved to" << getSavePath();
}


void MacroManager::loadMacros()
{
    QFile loadFile(getSavePath());
    if (!loadFile.open(QIODevice::ReadOnly)) {
        // This is not an error on first launch, just means no file exists yet
        qWarning() << "Couldn't open macros.json for reading. No macros loaded.";
        return;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(saveData));

    if (doc.isNull()) {
        qWarning() << "Failed to parse macros.json. File might be corrupt.";
        return;
    }

    // Get array of macros
    QJsonArray macrosArray = doc.object()["macros"].toArray();

    // Loop over every JSON object in the array
    for (const QJsonValue& val : macrosArray) {
        QJsonObject macroObj = val.toObject();

        // Create a new C++ Macro object
        QSharedPointer<Macro> macro = QSharedPointer<Macro>::create();
        macro->name = macroObj["name"].toString();
        macro->shortcut = macroObj["shortcut"].toString();

        QJsonArray actionsArray = macroObj["actions"].toArray();
        for (const QJsonValue& actionVal : actionsArray) {
            QJsonObject actionObj = actionVal.toObject();
            QString type = actionObj["type"].toString();

            if (type == "OpenAppAction") {
                macro->addAction(QSharedPointer<OpenAppAction>::create(actionObj["path"].toString()));
            } else if (type == "OpenURLAction") {
                macro->addAction(QSharedPointer<OpenURLAction>::create(actionObj["url"].toString()));
            } else if (type == "TypeKeystrokeAction") {
                macro->addAction(QSharedPointer<TypeKeystrokeAction>::create(actionObj["text"].toString()));
            } else if (type == "OpenVSCodeFolderAction") {
                macro->addAction(QSharedPointer<OpenVSCodeFolderAction>::create(actionObj["path"].toString()));
            } else if (type == "OpenFolderAction") {
                macro->addAction(QSharedPointer<OpenFolderAction>::create(actionObj["path"].toString()));
            } else if (type == "RunCommandAction") {
                macro->addAction(QSharedPointer<RunCommandAction>::create(actionObj["command"].toString()));
            } else {
                qWarning() << "Unknown action type in macros.json:" << type;
            }
        }

        // Add the fully-built macro to our list
        // Note: We don't call addMacro() because we don't want to trigger a re-save
        macros.append(macro);

        // We MUST emit macroAdded so the UI and HotkeyManager
        // will load this macro and register its hotkey.
        emit macroAdded(macro);
    }

    qDebug() << "Loaded" << macros.count() << "macros from" << getSavePath();
}


