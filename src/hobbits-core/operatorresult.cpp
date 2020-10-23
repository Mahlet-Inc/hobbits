#include "operatorresult.h"

OperatorResult::OperatorResult()
{

}

OperatorResult* OperatorResult::setOutputContainers(QList<QSharedPointer<BitContainer>> outputContainers)
{
    m_outputContainers = outputContainers;
    return this;
}

QList<QSharedPointer<BitContainer>> OperatorResult::outputContainers() const
{
    return m_outputContainers;
}

OperatorResult* OperatorResult::setParameters(QJsonObject parameters)
{
    m_parameters = parameters;
    return this;
}

const QJsonObject OperatorResult::parameters() const
{
    return m_parameters;
}

bool OperatorResult::hasEmptyParameters() const
{
    return m_parameters.isEmpty();
}

QString OperatorResult::errorString() const
{
    return m_errorString;
}

QSharedPointer<const OperatorResult> OperatorResult::result(QList<QSharedPointer<BitContainer>> outputContainers, QJsonObject pluginState)
{
    return QSharedPointer<const OperatorResult>(
                (new OperatorResult())->setOutputContainers(outputContainers)->setParameters(pluginState)
            );
}

QSharedPointer<const OperatorResult> OperatorResult::error(QString errorString)
{
    auto result = new OperatorResult();
    result->m_errorString = errorString;
    return QSharedPointer<const OperatorResult>(result);
}
