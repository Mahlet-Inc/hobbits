#ifndef PLUGINACTIONBATCH_H
#define PLUGINACTIONBATCH_H

#include "bitcontainer.h"
#include "hobbits-core_global.h"
#include "pluginactionlineage.h"

class HOBBITSCORESHARED_EXPORT PluginActionBatch : public QEnableSharedFromThis<PluginActionBatch>
{
public:
    enum Mode {
        Inclusive = 0x01,
        Before = 0x02,
        After = 0x04,
        ExclusiveBefore = 0x02,
        InclusiveBefore = 0x03,
        ExclusiveAfter = 0x04,
        InclusiveAfter = 0x05,
    };
    static QSharedPointer<PluginActionBatch> fromLineage(QSharedPointer<const PluginActionLineage> lineage, Mode batchMode);

    class ActionStep {
    public:
        ActionStep(QUuid id, QSharedPointer<const PluginAction> action) :
            stepId(id),
            action(action) {}

        QUuid stepId;
        QSharedPointer<const PluginAction> action;
        QList<QPair<QUuid, int>> inputs;
    };

    QJsonObject serialize() const;
    static QSharedPointer<PluginActionBatch> deserialize(QJsonObject data);

    int getMinRequiredInputs(QSharedPointer<const HobbitsPluginManager> pluginManager) const;
    int getMaxPossibleInputs(QSharedPointer<const HobbitsPluginManager> pluginManager) const;

    QList<QSharedPointer<const ActionStep>> actionSteps() const;

private:
    PluginActionBatch(QList<QSharedPointer<const ActionStep>> actionSteps);
    QList<QSharedPointer<const ActionStep>> m_actionSteps;
};

#endif // PLUGINACTIONBATCH_H
