#ifndef OPENVSCODEFOLDERACTION_H
#define OPENVSCODEFOLDERACTION_H

#include "action.h"
#include <QString>

class OpenVSCodeFolderAction : public Action
{
public:
    explicit OpenVSCodeFolderAction(const QString& folderPath);

    void execute() override;
    QString description() const override;
    QJsonObject toJson() const override;

private:
    QString folderPath;
};

#endif // OPENVSCODEFOLDERACTION_H
