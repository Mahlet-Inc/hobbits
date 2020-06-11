#include "pluginaction.h"
#include "pluginactionlineage.h"

#include "bitcontainermanager.h"

#include <QtConcurrent/QtConcurrentRun>

PluginAction::PluginAction(PluginType pluginType, QString pluginName, QJsonObject pluginState) :
    m_pluginType(pluginType),
    m_pluginName(pluginName),
    m_pluginState(pluginState)
{

}

PluginAction::PluginType PluginAction::getPluginType() const
{
    return m_pluginType;
}

QString PluginAction::getPluginName() const
{
    return m_pluginName;
}

QJsonObject PluginAction::getPluginState() const
{
    return m_pluginState;
}

int PluginAction::minPossibleInputs(QSharedPointer<const HobbitsPluginManager> pluginManager) const
{
    QSharedPointer<OperatorInterface> plugin = pluginManager->getOperator(m_pluginName);
    if (plugin.isNull()) {
        return 0;
    }
    return plugin->getMinInputContainers(m_pluginState);
}

int PluginAction::maxPossibleInputs(QSharedPointer<const HobbitsPluginManager> pluginManager) const
{
    QSharedPointer<OperatorInterface> plugin = pluginManager->getOperator(m_pluginName);
    if (plugin.isNull()) {
        return 0;
    }
    return plugin->getMaxInputContainers(m_pluginState);
}

QJsonObject PluginAction::serialize() const
{
    QJsonObject obj;

    obj.insert("type", m_pluginType);
    obj.insert("name", m_pluginName);
    obj.insert("state", m_pluginState);

    return obj;
}

QSharedPointer<PluginAction> PluginAction::deserialize(QJsonObject data)
{
    QSharedPointer<PluginAction> nullAction;

    if (!(data.contains("type")
          && data.contains("name")
          && data.contains("state"))) {
        return nullAction;
    }

    if (!(data.value("type").type() == QJsonValue::Type::Double
          && data.value("state").type() == QJsonValue::Type::Object
          && data.value("name").type() == QJsonValue::Type::String)) {
        return nullAction;
    }

    PluginType pluginType = static_cast<PluginType>(data.value("type").toInt());
    QString pluginName = data.value("name").toString();
    QJsonObject pluginState = data.value("state").toObject();

    return QSharedPointer<PluginAction>(new PluginAction(pluginType, pluginName, pluginState));
}

QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> PluginAction::operatorAct(
        QSharedPointer<OperatorActor> actor,
        QSharedPointer<const HobbitsPluginManager> pluginManager,
        QList<QSharedPointer<BitContainer>> inputContainers,
        QSharedPointer<BitContainerManager> bitContainerManager,
        QString outputName,
        QMap<int, QUuid> outputIdMap) const
{
    QSharedPointer<OperatorInterface> plugin = pluginManager->getOperator(m_pluginName);
    if (plugin.isNull()) {
        return QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>>();
    }
    if (!plugin->canRecallPluginState(m_pluginState)) {
        return QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>>();
    }

    return actor->operatorFullAct(
            plugin,
            inputContainers,
            bitContainerManager,
            outputName,
            m_pluginState,
            outputIdMap);
}

QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> PluginAction::analyzerAct(
        QSharedPointer<AnalyzerActor> actor,
        QSharedPointer<const HobbitsPluginManager> pluginManager,
        QSharedPointer<BitContainer> container) const
{
    QSharedPointer<AnalyzerInterface> plugin = pluginManager->getAnalyzer(m_pluginName);
    if (plugin.isNull()) {
        return QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>>();
    }
    if (!plugin->canRecallPluginState(m_pluginState)) {
        return QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>>();
    }

    return actor->analyzerFullAct(plugin, container, m_pluginState);
}


QSharedPointer<ImportExportResult> PluginAction::importAct(QSharedPointer<const HobbitsPluginManager> pluginManager, QWidget* parent) const
{
    QSharedPointer<ImportExportInterface> plugin = pluginManager->getImporterExporter(m_pluginName);
    if (plugin.isNull()) {
        return ImportExportResult::error(QString("Plugin '%1' could not be loaded"));
    }
    if (!plugin->canImport()) {
        return ImportExportResult::error(QString("Plugin '%1' can not be used to import data"));
    }

    return plugin->importBits(m_pluginState, parent);
}

QSharedPointer<ImportExportResult> PluginAction::exportAct(QSharedPointer<const HobbitsPluginManager> pluginManager, QSharedPointer<BitContainer> container, QWidget* parent) const
{
    QSharedPointer<ImportExportInterface> plugin = pluginManager->getImporterExporter(m_pluginName);
    if (plugin.isNull()) {
        return ImportExportResult::error(QString("Plugin '%1' could not be loaded"));
    }
    if (!plugin->canExport()) {
        return ImportExportResult::error(QString("Plugin '%1' can not be used to export data"));
    }

    return plugin->exportBits(container, m_pluginState, parent);
}
