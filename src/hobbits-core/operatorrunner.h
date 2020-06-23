#ifndef OPERATORACTOR_H
#define OPERATORACTOR_H

#include "actionwatcher.h"
#include "bitcontainermanager.h"
#include "hobbitspluginmanager.h"
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>
#include "bitcontainermanager.h"

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT OperatorRunner : public QObject
{
    Q_OBJECT

public:
    static QSharedPointer<OperatorRunner> create(
            QSharedPointer<const HobbitsPluginManager> pluginManager,
            QSharedPointer<BitContainerManager> containerManager,
            QSharedPointer<const PluginAction> action);

    QUuid id() const;
    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> getWatcher();

    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> run(QList<QSharedPointer<BitContainer>> inputContainers);

signals:
    void reportError(QUuid, QString);
    void progress(QUuid, int);
    void finished(QUuid);

private slots:
    void postProcess();

private:
    static QSharedPointer<const OperatorResult> operatorCall(
            QSharedPointer<OperatorInterface> op,
            QList<QSharedPointer<const BitContainer>> inputContainers,
            QJsonObject pluginState,
            QSharedPointer<ActionProgress> progressTracker);

    QUuid m_id;
    QSharedPointer<const PluginAction> m_action;
    QSharedPointer<OperatorInterface> m_op;
    QString m_pluginFileLocation;
    QList<QSharedPointer<BitContainer>> m_inputContainers;
    QList<QSharedPointer<BitContainer>> m_outputContainers;

    QSharedPointer<BitContainerManager> m_containerManager;

    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> m_actionWatcher;
};

#endif // OPERATORACTOR_H
