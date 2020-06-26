#ifndef PLUGINACTION_H
#define PLUGINACTION_H

#include "actionwatcher.h"
#include "analyzerrunner.h"
#include "hobbitspluginmanager.h"
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
        Analyzer = 3,
        Importer = 4,
        Exporter = 5,
        NoAction = 6
    };

    PluginAction(PluginType pluginType, QString pluginName, QJsonObject pluginState);

    static QSharedPointer<PluginAction> analyzerAction(QString pluginName, QJsonObject pluginState);
    static QSharedPointer<PluginAction> operatorAction(QString pluginName, QJsonObject pluginState);
    static QSharedPointer<PluginAction> importerAction(QString pluginName, QJsonObject pluginState = QJsonObject());
    static QSharedPointer<PluginAction> exporterAction(QString pluginName, QJsonObject pluginState = QJsonObject());
    static QSharedPointer<PluginAction> noAction();

    PluginType getPluginType() const;
    QString getPluginName() const;
    QJsonObject getPluginState() const;

    int minPossibleInputs(QSharedPointer<const HobbitsPluginManager> pluginManager) const;
    int maxPossibleInputs(QSharedPointer<const HobbitsPluginManager> pluginManager) const;

    QJsonObject serialize() const;

    static QSharedPointer<PluginAction> deserialize(QJsonObject data);

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
