#ifndef OPERATORRESULT_H
#define OPERATORRESULT_H

#include <QJsonObject>
#include <QList>
#include <QSharedPointer>

#include "bitcontainer.h"

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT OperatorResult
{
public:
    OperatorResult();

    OperatorResult* setOutputContainers(QList<QSharedPointer<BitContainer>> outputContainers);
    QList<QSharedPointer<BitContainer>> getOutputContainers() const;

    OperatorResult* setPluginState(QJsonObject pluginState);
    const QJsonObject getPluginState() const;

private:
    QList<QSharedPointer<BitContainer>> m_outputContainers;
    int m_maxFrameWidth;

    QJsonObject m_pluginState;
};

#endif // OPERATORRESULT_H
