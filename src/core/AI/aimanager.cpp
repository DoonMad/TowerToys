#include "aimanager.h"
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>
#include "Actions/openappaction.h"
#include "Actions/openurlaction.h"
#include "Actions/runcommandaction.h"
#include "Actions/delayaction.h"
#include "Actions/openfolderaction.h"
#include "Actions/openvscodefolderaction.h"
#include "Actions/typekeystrokeaction.h"

AIManager::AIManager(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &AIManager::onReplyFinished);
}

bool AIManager::generateMacro(const QString &prompt)
{
    QSettings settings;
    QString apiKey = settings.value("geminiApiKey", "").toString();
    if (apiKey.isEmpty()) {
        return false;
    }

    QUrl url("https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=" + apiKey);
    qDebug() << "url: https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" << apiKey;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString systemInstruction = 
        "You are an assistant that generates automation Macros. "
        "The user will describe a task. You must output ONLY a raw JSON object representing the Macro, with no markdown formatting and no backticks. "
        "The JSON object MUST have the following structure:\n"
        "{\n"
        "  \"name\": \"A short, descriptive name for the macro\",\n"
        "  \"actions\": [\n"
        "    {\n"
        "      \"type\": \"One of: Open App, Open URL, Run Command, Delay, Open Folder, Open Folder in VS Code, Type Keystroke\",\n"
        "      // Based on the type, provide the corresponding property:\n"
        "      // For 'Open App', provide 'path' (executable path).\n"
        "      // For 'Open URL', provide 'url' (e.g. http://google.com).\n"
        "      // For 'Run Command', provide 'command' (cmd/powershell command).\n"
        "      // For 'Delay', provide 'delayMs' (integer milliseconds).\n"
        "      // For 'Open Folder', provide 'path' (directory path).\n"
        "      // For 'Open Folder in VS Code', provide 'path' (directory path).\n"
        "      // For 'Type Keystroke', provide 'text' (string to type).\n"
        "    }\n"
        "  ]\n"
        "}\n"
        "Generate a logical sequence of actions based on the user's prompt. If the user doesn't provide exact paths, guess a placeholder like 'C:\\\\path\\\\to\\\\app.exe' or 'C:\\\\Projects\\\\CodeChef' so they can edit it later.";

    QJsonObject payload;
    
    // Use system_instruction feature of Gemini API for better adherence
    QJsonObject sysInstructionObj;
    QJsonArray sysParts;
    QJsonObject sysPart;
    sysPart["text"] = systemInstruction;
    sysParts.append(sysPart);
    sysInstructionObj["parts"] = sysParts;
    payload["system_instruction"] = sysInstructionObj;

    QJsonArray contents;
    QJsonObject contentItem;
    QJsonObject parts;
    parts["text"] = prompt;
    QJsonArray partsArray;
    partsArray.append(parts);
    contentItem["parts"] = partsArray;
    contents.append(contentItem);
    payload["contents"] = contents;

    QJsonDocument doc(payload);
    networkManager->post(request, doc.toJson());

    return true;
}

void AIManager::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QByteArray errorBody = reply->readAll();
        emit generationFailed("Network error: " + reply->errorString() + "\nServer Reply: " + QString::fromUtf8(errorBody));
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    
    QJsonObject root = jsonDoc.object();
    QJsonArray candidates = root["candidates"].toArray();
    if (candidates.isEmpty()) {
        emit generationFailed("Empty response from AI.");
        reply->deleteLater();
        return;
    }

    QJsonObject firstCandidate = candidates[0].toObject();
    QJsonObject content = firstCandidate["content"].toObject();
    QJsonArray parts = content["parts"].toArray();
    if (parts.isEmpty()) {
        emit generationFailed("No text parts in AI response.");
        reply->deleteLater();
        return;
    }

    QString textResult = parts[0].toObject()["text"].toString();
    textResult = textResult.trimmed();
    
    if (textResult.startsWith("```json")) {
        textResult.remove(0, 7);
    } else if (textResult.startsWith("```")) {
        textResult.remove(0, 3);
    }
    if (textResult.endsWith("```")) {
        textResult.remove(textResult.length() - 3, 3);
    }
    textResult = textResult.trimmed();

    QJsonParseError parseError;
    QJsonDocument macroDoc = QJsonDocument::fromJson(textResult.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        emit generationFailed("Failed to parse AI JSON output: " + parseError.errorString() + "\nRaw:\n" + textResult);
        reply->deleteLater();
        return;
    }

    QJsonObject macroObj = macroDoc.object();
    QSharedPointer<Macro> newMacro = QSharedPointer<Macro>::create();
    newMacro->name = macroObj["name"].toString("AI Generated Macro");

    QJsonArray actionsArray = macroObj["actions"].toArray();
    for (int i = 0; i < actionsArray.size(); ++i) {
        QJsonObject actionObj = actionsArray[i].toObject();
        QString type = actionObj["type"].toString();

        if (type == "Open App") {
            newMacro->addAction(QSharedPointer<OpenAppAction>::create(actionObj["path"].toString()));
        } else if (type == "Open URL") {
            newMacro->addAction(QSharedPointer<OpenURLAction>::create(actionObj["url"].toString()));
        } else if (type == "Run Command") {
            newMacro->addAction(QSharedPointer<RunCommandAction>::create(actionObj["command"].toString()));
        } else if (type == "Delay") {
            newMacro->addAction(QSharedPointer<DelayAction>::create(actionObj["delayMs"].toInt(500)));
        } else if (type == "Open Folder") {
            newMacro->addAction(QSharedPointer<OpenFolderAction>::create(actionObj["path"].toString()));
        } else if (type == "Open Folder in VS Code") {
            newMacro->addAction(QSharedPointer<OpenVSCodeFolderAction>::create(actionObj["path"].toString()));
        } else if (type == "Type Keystroke") {
            newMacro->addAction(QSharedPointer<TypeKeystrokeAction>::create(actionObj["text"].toString()));
        }
    }

    emit macroGenerated(newMacro);
    reply->deleteLater();
}
