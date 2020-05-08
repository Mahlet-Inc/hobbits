#ifndef OPERATORACTOR_H
#define OPERATORACTOR_H

#include "actionwatcher.h"
#include "bitcontainermanager.h"
#include "hobbitspluginmanager.h"
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>

#include "hobbits-core_global.h"

class PluginActionManager;

class HOBBITSCORESHARED_EXPORT OperatorActor : public QObject
{
    Q_OBJECT

public:
    explicit OperatorActor(PluginActionManager *manager, QSharedPointer<const HobbitsPluginManager> pluginManager);

    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> operatorFullAct(
            QSharedPointer<OperatorInterface> op,
            QList<QSharedPointer<BitContainer>> inputContainers,
            QSharedPointer<BitContainerManager> bitContainerManager,
            QString outputName,
            QJsonObject pluginState = QJsonObject(),
            QMap<int, QUuid> outputIdMap = QMap<int, QUuid>());

signals:
    void reportError(QString);

public slots:
private slots:
    void postProcess();

private:
    static QSharedPointer<const OperatorResult> operatorCall(
            QSharedPointer<OperatorInterface> op,
            QList<QSharedPointer<const BitContainer>> inputContainers,
            QJsonObject pluginState,
            QSharedPointer<ActionProgress> progressTracker);

    QFuture<QSharedPointer<const OperatorResult>> m_future;
    QSharedPointer<OperatorInterface> m_op;
    QList<QSharedPointer<BitContainer>> m_inputContainers;
    QSharedPointer<BitContainerManager> m_bitContainerManager;
    QSharedPointer<const HobbitsPluginManager> m_pluginManager;
    QString m_outputName;
    QJsonObject m_pluginState;
    QMap<int, QUuid> m_outputIdMap;

    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> m_actionWatcher;

    PluginActionManager *m_manager;

};

#endif // OPERATORACTOR_H
