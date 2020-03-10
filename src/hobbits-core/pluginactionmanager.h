#ifndef PLUGINACTIONMANAGER_H
#define PLUGINACTIONMANAGER_H

#include <QObject>
#include <QQueue>
#include <QStack>

#include "actionwatcher.h"
#include "analyzeractor.h"
#include "operatoractor.h"
#include "pluginmanager.h"
#include <QSharedPointer>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT PluginActionManager : public QObject
{
    Q_OBJECT

public:
    PluginActionManager(QSharedPointer<const PluginManager> pluginManager);

    struct LineageAction {
        QUuid containerId;
        QList<QSharedPointer<const PluginActionLineage>> lineage;
        QList<QSharedPointer<BitContainer>> inputs;
        QList<QSharedPointer<BitContainer>> additionalInputs;
        QSharedPointer<BitContainerManager> bitContainerManager;
        QString baseName;
        QMap<int, QUuid> outputIdOverride;
        QList<QUuid> additionalInputIds;
        int step;
        int additionalStep;
    };

    bool isBusy() const;
    bool hasActiveLineage() const;

    bool registerOperatorWatcher(QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> watcher);
    bool registerAnalyzerWatcher(QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> watcher);

    void applyLineage(
            QUuid containerId,
            QSharedPointer<const PluginActionLineage> lineage,
            QSharedPointer<BitContainerManager> bitContainerManager,
            QString baseName,
            QMap<int, QUuid> outputIdOverride = QMap<int, QUuid>(),
            QList<QUuid> additionalInputs = QList<QUuid>());

    void applyLineage(QSharedPointer<LineageAction> lineageAction);

    QSharedPointer<OperatorActor> operatorActor();
    QSharedPointer<AnalyzerActor> analyzerActor();

private slots:
    void operatorWatcherFinished();
    void analyzerWatcherFinished();

public slots:
    void cancelAction();

signals:
    void actionWatcherStarted();
    void actionWatcherProgress(int);
    void actionWatcherFinished();
    void lineageQueueCompleted();

    void reportError(QString);

private:
    void continueLineage();
    void finishLineage();

    QSharedPointer<const PluginManager> m_pluginManager;

    QSharedPointer<OperatorActor> m_operatorActor;
    QSharedPointer<AnalyzerActor> m_analyzerActor;

    QSharedPointer<LineageAction> m_current;

    QStack<QSharedPointer<LineageAction>> m_lineageStack;

    QQueue<QSharedPointer<LineageAction>> m_lineageQueue;

    QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> m_currOperatorWatcher;
    QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> m_currAnalyzerWatcher;

};

#endif // PLUGINACTIONMANAGER_H
