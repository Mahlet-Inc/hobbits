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
    struct TreeNode {
        QSharedPointer<const PluginActionLineage> lineage;
        QList<QSharedPointer<TreeNode>> children;
    };

    static QSharedPointer<PluginActionLineage> create(QSharedPointer<const PluginAction> pluginAction);
    static QSharedPointer<const PluginActionLineage> fromLineage(
            QSharedPointer<const PluginActionLineage> lineage,
            int offset = 0,
            int length = -1);

    static QSharedPointer<TreeNode> mergeIntoTree(QList<QSharedPointer<const PluginActionLineage>> branchingLineages);

    QSharedPointer<PluginActionLineage> setParent(QSharedPointer<const PluginActionLineage> parent);
    QSharedPointer<PluginActionLineage> setOutputPos(int outputPosition);

    QSharedPointer<const PluginAction> getPluginAction() const;
    int getOutputPosition() const;

    QJsonObject serialize() const;
    static QSharedPointer<PluginActionLineage> deserialize(QJsonObject data);

    QList<QSharedPointer<const PluginActionLineage>> getLineage() const;

private:
    QSharedPointer<const PluginAction> m_pluginAction;
    int m_outputPosition;
    QSharedPointer<const PluginActionLineage> m_parent;
};

#endif // PLUGINACTIONLINEAGE_H
