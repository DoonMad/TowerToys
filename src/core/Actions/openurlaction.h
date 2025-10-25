#ifndef OPENURLACTION_H
#define OPENURLACTION_H

#include "action.h"
#include <QString>
#include <QJsonObject>

class OpenURLAction : public Action
{
public:
    OpenURLAction(const QString& url);
    void execute() override;
    QString description() const override;
    QJsonObject toJson() const override;
    QString getUrl() const { return url; }
    void setUrl(const QString& path) { url = path; }

private:
    QString url;
};

#endif // OPENURLACTION_H
