#include "delayaction.h"
#include <QThread>
#include <QDebug>

DelayAction::DelayAction(int delayMs) : m_delayMs(delayMs)
{
}

void DelayAction::execute()
{
    qDebug() << "Executing DelayAction for" << m_delayMs << "ms";
    QThread::msleep(m_delayMs);
}

QString DelayAction::description() const
{
    return QString("Delay for %1 ms").arg(m_delayMs);
}

QJsonObject DelayAction::toJson() const
{
    QJsonObject obj;
    obj["type"] = "Delay";
    obj["delayMs"] = m_delayMs;
    return obj;
}

int DelayAction::getDelayMs() const
{
    return m_delayMs;
}

void DelayAction::setDelayMs(int delayMs)
{
    m_delayMs = delayMs;
}
