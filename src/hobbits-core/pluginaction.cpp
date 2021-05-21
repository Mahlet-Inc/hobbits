#include "pluginaction.h"
#include "pluginactionlineage.h"

#include "bitcontainermanager.h"

#include <QtConcurrent/QtConcurrentRun>

PluginAction::PluginAction(PluginType pluginType, QString pluginName, Parameters parameters) :
    m_pluginType(pluginType),
    m_pluginName(pluginName),
    m_parameters(parameters)
{

}

QSharedPointer<PluginAction> PluginAction::createAction(PluginAction::PluginType pluginType, QString pluginName, const Parameters &parameters)
{
    return QSharedPointer<PluginAction>(new PluginAction(pluginType, pluginName, parameters));
}

QSharedPointer<PluginAction> PluginAction::analyzerAction(QString pluginName, const Parameters &parameters)
{
    return QSharedPointer<PluginAction>(new PluginAction(PluginAction::Analyzer, pluginName, parameters));
}

QSharedPointer<PluginAction> PluginAction::operatorAction(QString pluginName, const Parameters &parameters)
{
    return QSharedPointer<PluginAction>(new PluginAction(PluginAction::Operator, pluginName, parameters));
}

QSharedPointer<PluginAction> PluginAction::importerAction(QString pluginName, const Parameters &parameters)
{
    return QSharedPointer<PluginAction>(new PluginAction(PluginAction::Importer, pluginName, parameters));
}

QSharedPointer<PluginAction> PluginAction::exporterAction(QString pluginName, const Parameters &parameters)
{
    return QSharedPointer<PluginAction>(new PluginAction(PluginAction::Exporter, pluginName, parameters));
}

QSharedPointer<PluginAction> PluginAction::noAction()
{
    return QSharedPointer<PluginAction>(new PluginAction(PluginAction::NoAction, "No Action", Parameters::nullParameters()));
}

PluginAction::PluginType PluginAction::pluginType() const
{
    return m_pluginType;
}

QString PluginAction::pluginName() const
{
    return m_pluginName;
}

Parameters PluginAction::parameters() const
{
    return m_parameters;
}

int PluginAction::minPossibleInputs(QSharedPointer<const HobbitsPluginManager> pluginManager) const
{
    QSharedPointer<OperatorInterface> plugin = pluginManager->getOperator(m_pluginName);
    if (plugin.isNull()) {
        return 0;
    }
    return plugin->getMinInputContainers(m_parameters);
}

int PluginAction::maxPossibleInputs(QSharedPointer<const HobbitsPluginManager> pluginManager) const
{
    QSharedPointer<OperatorInterface> plugin = pluginManager->getOperator(m_pluginName);
    if (plugin.isNull()) {
        return 0;
    }
    return plugin->getMaxInputContainers(m_parameters);
}

QJsonObject PluginAction::serialize() const
{
    QJsonObject obj;

    obj.insert("type", m_pluginType);
    obj.insert("name", m_pluginName);
    obj.insert("state", m_parameters.serialize());

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
    Parameters parameters = Parameters::deserialize(data.value("state"));

    return QSharedPointer<PluginAction>(new PluginAction(pluginType, pluginName, parameters));
}
