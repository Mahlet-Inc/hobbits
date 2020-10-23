#include "exportresult.h"

ExportResult::ExportResult()
{

}

ExportResult* ExportResult::setParameters(QJsonObject pluginState)
{
    m_parameters = pluginState;
    return this;
}

const QJsonObject ExportResult::parameters() const
{
    return m_parameters;
}

bool ExportResult::hasEmptyParameters() const
{
    return m_parameters.isEmpty();
}

QString ExportResult::errorString() const
{
    return m_errorString;
}

QSharedPointer<ExportResult> ExportResult::nullResult()
{
    return QSharedPointer<ExportResult>(new ExportResult());
}

QSharedPointer<ExportResult> ExportResult::error(QString errorString)
{
    auto result = new ExportResult();
    result->m_errorString = errorString;
    return QSharedPointer<ExportResult>(result);
}

QSharedPointer<ExportResult> ExportResult::result(QJsonObject parameters)
{
    return QSharedPointer<ExportResult>((new ExportResult())->setParameters(parameters));
}
