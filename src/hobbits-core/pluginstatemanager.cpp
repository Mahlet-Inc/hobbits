#include "pluginstatemanager.h"
#include <QJsonObject>

PluginStateManager::PluginStateManager(QVector<PluginStateManager::Parameter> parameters)
{
    for (auto parameter : parameters) {
        m_parameterMap.insert(parameter.name, parameter);
    }
}

QSharedPointer<PluginStateEditor> PluginStateManager::createEditor(QSize targetBounds)
{
    Q_UNUSED(targetBounds)
    return QSharedPointer<PluginStateEditor>();
}

QList<PluginStateManager::Parameter> PluginStateManager::parameters() const
{
    return m_parameterMap.values();
}

bool PluginStateManager::validate(const QJsonObject &pluginState) const
{
    if (pluginState.isEmpty()) {
        return false;
    }

    for (auto param : parameters()) {
        if (!pluginState.contains(param.name)) {
            if (!param.optional) {
                return false;
            }
        }
        else if (param.type != pluginState.value(param.name).type()) {
            return false;
        }
    }
    return true;
}
