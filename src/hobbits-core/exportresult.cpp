#include "exportresult.h"

ExportResult::ExportResult()
{

}

ExportResult* ExportResult::setParameters(const Parameters &parameters)
{
    m_parameters = parameters;
    return this;
}

const Parameters ExportResult::parameters() const
{
    return m_parameters;
}

bool ExportResult::hasNullParameters() const
{
    return m_parameters.isNull();
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

QSharedPointer<ExportResult> ExportResult::result(Parameters parameters)
{
    return QSharedPointer<ExportResult>((new ExportResult())->setParameters(parameters));
}
