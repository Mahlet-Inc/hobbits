#ifndef PLUGINACTION_H
#define PLUGINACTION_H

#include "actionwatcher.h"
#include "analyzeractor.h"
#include "operatoractor.h"
#include "pluginmanager.h"
#include <QJsonObject>
#include <QtConcurrent/QtConcurrentRun>

#include "hobbits-core_global.h"

class BitContainerManager;
class HOBBITSCORESHARED_EXPORT PluginAction
{
public:
    enum PluginType {
        Framer = 1 /*Deprecated*/,
        Operator = 2,
        Analyzer = 3
    };

    PluginAction(PluginType pluginType, QString pluginName, QJsonObject pluginState);

    PluginType getPluginType() const;
    QString getPluginName() const;
    QJsonObject getPluginState() const;

    QJsonObject serialize() const;

    static QSharedPointer<PluginAction> deserialize(QJsonObject data);

    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> operatorAct(
            QSharedPointer<OperatorActor> actor,
            QSharedPointer<const PluginManager> pluginManager,
            QList<QSharedPointer<BitContainer>> inputContainers,
            QSharedPointer<BitContainerManager> bitContainerManager,
            QString outputName = "",
            QMap<int, QUuid> outputIdMap = QMap<int, QUuid>()) const;
    QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> analyzerAct(
            QSharedPointer<AnalyzerActor> actor,
            QSharedPointer<const PluginManager> pluginManager,
            QSharedPointer<BitContainer> container) const;

    inline bool operator==(const PluginAction &other) const
    {
        return (
            m_pluginName == other.getPluginName()
            && m_pluginType == other.getPluginType()
            && m_pluginState == other.getPluginState()
            );
    }

private:
    PluginType m_pluginType;
    QString m_pluginName;
    QJsonObject m_pluginState;
};


inline uint qHash(const PluginAction &key, uint seed)
{
    return qHash(key.getPluginState(), seed) ^ uint(key.getPluginType()) ^ qHash(key.getPluginName(), seed);
}

#endif // PLUGINACTION_H
