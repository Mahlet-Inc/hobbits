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

class HOBBITSCORESHARED_EXPORT OperatorRunner : public QObject
{
    Q_OBJECT

public:
    static QSharedPointer<OperatorRunner> create(
            QSharedPointer<const HobbitsPluginManager> pluginManager,
            QSharedPointer<PluginAction> action);

    QUuid id() const;
    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> getWatcher();

    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> run(QList<QSharedPointer<BitContainer>> inputContainers);

signals:
    void reportError(QString);
    void finished(QUuid);
    void finishedFail(QUuid, QString);

private slots:
    void postProcess();

private:
    static QSharedPointer<const OperatorResult> operatorCall(
            QSharedPointer<OperatorInterface> op,
            QList<QSharedPointer<const BitContainer>> inputContainers,
            QJsonObject pluginState,
            QSharedPointer<ActionProgress> progressTracker);

    QUuid m_id;
    QSharedPointer<PluginAction> m_action;
    QSharedPointer<OperatorInterface> m_op;
    QString m_pluginFileLocation;
    QList<QSharedPointer<BitContainer>> m_inputContainers;
    QList<QSharedPointer<BitContainer>> m_outputContainers;

    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> m_actionWatcher;
};

#endif // OPERATORACTOR_H
