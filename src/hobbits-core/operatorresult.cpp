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


QSharedPointer<const OperatorResult> OperatorResult::result(QList<QSharedPointer<BitContainer>> outputContainers, QJsonObject pluginState)
{
    return QSharedPointer<const OperatorResult>(
                (new OperatorResult())->setOutputContainers(outputContainers)->setPluginState(pluginState)
            );
}

QSharedPointer<const OperatorResult> OperatorResult::error(QString error)
{
    return QSharedPointer<const OperatorResult>(
            (new OperatorResult())->setPluginState(
                    QJsonObject(
                            {QPair<QString, QJsonValue>(
                                    "error",
                                    QJsonValue(error))}))
            );
}
