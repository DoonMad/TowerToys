#ifndef DELAYACTION_H
#define DELAYACTION_H

#include "action.h"

class DelayAction : public Action
{
public:
    explicit DelayAction(int delayMs);
    void execute() override;
    QString description() const override;
    QJsonObject toJson() const override;
    int getDelayMs() const;
    void setDelayMs(int delayMs);

private:
    int m_delayMs;
};

#endif // DELAYACTION_H
