#include "exportresult.h"

ExportResult::ExportResult()
{

}

ExportResult* ExportResult::setPluginState(QJsonObject pluginState)
{
    m_pluginState = pluginState;
    return this;
}

const QJsonObject ExportResult::pluginState() const
{
    return m_pluginState;
}

bool ExportResult::hasEmptyState() const
{
    return m_pluginState.isEmpty();
}

QString ExportResult::errorString() const
{
    if (m_pluginState.contains("error")) {
        return m_pluginState.value("error").toString();
    }
    return QString();
}

QSharedPointer<ExportResult> ExportResult::nullResult()
{
    return QSharedPointer<ExportResult>(new ExportResult());
}

QSharedPointer<ExportResult> ExportResult::error(QString error)
{
    QJsonObject pluginState;
    pluginState.insert("error", error);
    return QSharedPointer<ExportResult>((new ExportResult())->setPluginState(pluginState));
}

QSharedPointer<ExportResult> ExportResult::result(QJsonObject pluginState)
{
    return QSharedPointer<ExportResult>((new ExportResult())->setPluginState(pluginState));
}
