#include "importresult.h"

ImportResult::ImportResult()
{

}

ImportResult* ImportResult::setContainer(QSharedPointer<BitContainer> container)
{
    m_container = container;
    return this;
}

QSharedPointer<BitContainer> ImportResult::getContainer() const
{
    return m_container;
}

ImportResult* ImportResult::setPluginState(QJsonObject pluginState)
{
    m_pluginState = pluginState;
    return this;
}

const QJsonObject ImportResult::pluginState() const
{
    return m_pluginState;
}

bool ImportResult::hasEmptyState() const
{
    return m_pluginState.isEmpty();
}

QString ImportResult::errorString() const
{
    if (m_pluginState.contains("error")) {
        return m_pluginState.value("error").toString();
    }
    return QString();
}

QSharedPointer<ImportResult> ImportResult::nullResult()
{
    return QSharedPointer<ImportResult>(new ImportResult());
}

QSharedPointer<ImportResult> ImportResult::error(QString error)
{
    QJsonObject pluginState;
    pluginState.insert("error", error);
    return QSharedPointer<ImportResult>((new ImportResult())->setPluginState(pluginState));
}

QSharedPointer<ImportResult> ImportResult::result(QSharedPointer<BitContainer> container, QJsonObject pluginState)
{
    return QSharedPointer<ImportResult>((new ImportResult())->setContainer(container)->setPluginState(pluginState));
}
