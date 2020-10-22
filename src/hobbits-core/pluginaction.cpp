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

QSharedPointer<PluginAction> PluginAction::createAction(PluginAction::PluginType pluginType, QString pluginName, QJsonObject pluginState)
{
    return QSharedPointer<PluginAction>(new PluginAction(pluginType, pluginName, pluginState));
}

QSharedPointer<PluginAction> PluginAction::analyzerAction(QString pluginName, QJsonObject pluginState)
{
    return QSharedPointer<PluginAction>(new PluginAction(PluginAction::Analyzer, pluginName, pluginState));
}

QSharedPointer<PluginAction> PluginAction::operatorAction(QString pluginName, QJsonObject pluginState)
{
    return QSharedPointer<PluginAction>(new PluginAction(PluginAction::Operator, pluginName, pluginState));
}

QSharedPointer<PluginAction> PluginAction::importerAction(QString pluginName, QJsonObject pluginState)
{
    return QSharedPointer<PluginAction>(new PluginAction(PluginAction::Importer, pluginName, pluginState));
}

QSharedPointer<PluginAction> PluginAction::exporterAction(QString pluginName, QJsonObject pluginState)
{
    return QSharedPointer<PluginAction>(new PluginAction(PluginAction::Exporter, pluginName, pluginState));
}

QSharedPointer<PluginAction> PluginAction::noAction()
{
    return QSharedPointer<PluginAction>(new PluginAction(PluginAction::NoAction, "No Action", QJsonObject()));
}

PluginAction::PluginType PluginAction::pluginType() const
{
    return m_pluginType;
}

QString PluginAction::pluginName() const
{
    return m_pluginName;
}

QJsonObject PluginAction::parameters() const
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
