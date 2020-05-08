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
    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> nullResult;
    QSharedPointer<OperatorInterface> plugin = pluginManager->getOperator(m_pluginName);
    if (plugin.isNull()) {
        return nullResult;
    }
    if (!plugin->canRecallPluginState(m_pluginState)) {
        return nullResult;
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
    QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> nullResult;
    QSharedPointer<AnalyzerInterface> plugin = pluginManager->getAnalyzer(m_pluginName);
    if (plugin.isNull()) {
        return nullResult;
    }
    if (!plugin->canRecallPluginState(m_pluginState)) {
        return nullResult;
    }

    return actor->analyzerFullAct(plugin, container, m_pluginState);
}
