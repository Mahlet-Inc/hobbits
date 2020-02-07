#include "operatorresult.h"

OperatorResult::OperatorResult()
{

}

OperatorResult* OperatorResult::setOutputContainers(QList<QSharedPointer<BitContainer>> outputContainers)
{
    m_outputContainers = outputContainers;
    return this;
}

QList<QSharedPointer<BitContainer>> OperatorResult::getOutputContainers() const
{
    return m_outputContainers;
}

OperatorResult* OperatorResult::setPluginState(QJsonObject pluginState)
{
    m_pluginState = pluginState;
    return this;
}

const QJsonObject OperatorResult::getPluginState() const
{
    return m_pluginState;
}
