#ifndef OPENFOLDERACTION_H
#define OPENFOLDERACTION_H

#include "action.h"
#include <QString>

class OpenFolderAction : public Action
{
public:
    explicit OpenFolderAction(const QString& path);
    void execute() override;
    QString description() const override;
    QJsonObject toJson() const override;
    QString getPath() const { return folderPath; }
    void setPath(const QString& path) { folderPath = path; }
private:
    QString folderPath;
};
#endif // OPENFOLDERACTION_H
