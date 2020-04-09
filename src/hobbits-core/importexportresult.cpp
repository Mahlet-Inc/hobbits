#include "importexportresult.h"

ImportExportResult::ImportExportResult()
{

}

ImportExportResult* ImportExportResult::setContainer(QSharedPointer<BitContainer> container)
{
    m_container = container;
    return this;
}

QSharedPointer<BitContainer> ImportExportResult::getContainer() const
{
    return m_container;
}

ImportExportResult* ImportExportResult::setPluginState(QJsonObject pluginState)
{
    m_pluginState = pluginState;
    return this;
}

const QJsonObject ImportExportResult::getPluginState() const
{
    return m_pluginState;
}


QSharedPointer<ImportExportResult> ImportExportResult::nullResult()
{
    return QSharedPointer<ImportExportResult>();
}

QSharedPointer<ImportExportResult> ImportExportResult::error(QString error)
{
    QJsonObject pluginState;
    pluginState.insert("error", error);
    return QSharedPointer<ImportExportResult>((new ImportExportResult())->setPluginState(pluginState));
}

QSharedPointer<ImportExportResult> ImportExportResult::create(QSharedPointer<BitContainer> container, QJsonObject pluginState)
{
    return QSharedPointer<ImportExportResult>((new ImportExportResult())->setContainer(container)->setPluginState(pluginState));
}

QSharedPointer<ImportExportResult> ImportExportResult::create(QJsonObject pluginState)
{
    return QSharedPointer<ImportExportResult>((new ImportExportResult())->setPluginState(pluginState));
}
