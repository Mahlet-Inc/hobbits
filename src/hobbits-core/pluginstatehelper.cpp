#include "pluginstatehelper.h"
#include <QJsonObject>

PluginStateHelper::PluginStateHelper()
{

}
void PluginStateHelper::addParameter(QString name,
                  QJsonValue::Type type,
                  const std::function<bool(QJsonValue)> setInUi,
                  const std::function<QJsonValue()> getFromUi,
                  bool optional)
{
    m_parameters.append(QSharedPointer<Parameter>(new Parameter(name, type, setInUi, getFromUi, optional)));
}

bool PluginStateHelper::validatePluginState(const QJsonObject &pluginState)
{
    if (pluginState.isEmpty()) {
        return false;
    }

    for (auto param : m_parameters) {
        if (!pluginState.contains(param->name)) {
            if (!param->optional) {
                return false;
            }
        }
        else if (param->type != pluginState.value(param->name).type()) {
            return false;
        }
    }
    return true;
}

bool PluginStateHelper::applyPluginStateToUi(const QJsonObject &pluginState)
{
    if (!validatePluginState(pluginState)) {
        return false;
    }

    for (auto param : m_parameters) {
        if (!pluginState.contains(param->name)) {
            continue;
        }
        if (!param->setInUi(pluginState.value(param->name))) {
            return false;
        }
    }

    return true;
}

QJsonObject PluginStateHelper::getPluginStateFromUi()
{
    QJsonObject pluginState;
    for (auto param : m_parameters) {
        QJsonValue value = param->getFromUi();
        if (value.isUndefined()) {
            return QJsonObject();
        }
        pluginState.insert(param->name, value);
    }
    return pluginState;
}
