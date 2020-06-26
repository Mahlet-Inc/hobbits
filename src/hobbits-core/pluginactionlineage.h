#ifndef PLUGINACTIONLINEAGE_H
#define PLUGINACTIONLINEAGE_H

#include "pluginaction.h"
#include <QSharedPointer>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT PluginActionLineage : public QEnableSharedFromThis<PluginActionLineage>
{
private:
    PluginActionLineage(QSharedPointer<const PluginAction> pluginAction);

public:
    static void recordLineage(
                    QSharedPointer<const PluginAction> pluginAction,
                    QList<QSharedPointer<BitContainer>> inputContainers,
                    QList<QSharedPointer<BitContainer>> outputContainers);

    static QSharedPointer<PluginActionLineage> actionlessLineage();

    QSharedPointer<PluginActionLineage> setOutputPosition(int outputPosition);
    QSharedPointer<PluginActionLineage> addInput(QSharedPointer<const PluginActionLineage> input);
    QSharedPointer<PluginActionLineage> addOutputGroup(QList<QWeakPointer<const PluginActionLineage>> outputs);

    QSharedPointer<const PluginAction> getPluginAction() const;
    int getOutputPosition() const;
    QList<QSharedPointer<const PluginActionLineage>> getInputs() const;
    QList<QList<QWeakPointer<const PluginActionLineage>>> getOutputs() const;

    QJsonObject serialize() const;
    static QSharedPointer<PluginActionLineage> deserialize(QJsonObject data);

    QList<QSharedPointer<const PluginAction>> outputOperators() const;

private:
    QSharedPointer<const PluginAction> m_pluginAction;
    QList<QSharedPointer<const PluginActionLineage>> m_inputs;
    QList<QList<QWeakPointer<const PluginActionLineage>>> m_outputs;
    int m_outputPosition;
};

#endif // PLUGINACTIONLINEAGE_H
