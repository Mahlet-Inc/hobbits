#ifndef BATCHRUNNER_H
#define BATCHRUNNER_H

#include <QObject>
#include "pluginactionbatch.h"
#include "operatorrunner.h"

class PluginActionManager;

class BatchRunner : public QObject
{
    Q_OBJECT
public:
    static QSharedPointer<BatchRunner> create(QSharedPointer<PluginActionBatch>, QList<QSharedPointer<BitContainer>>);

    QUuid id() const;
    QStringList errorList() const;
    bool isRunning() const;

    void run(QSharedPointer<PluginActionManager> actionManager);

signals:
    void finished(QUuid);

public slots:
    void cancel();
    void checkDone();
    void checkFinishedAnalyzer(QUuid);
    void checkFinishedOperator(QUuid);

private slots:
    void checkForRunnableSteps();

private:
    explicit BatchRunner();

    QList<QPair<QUuid, int>> getStepInputs(QSharedPointer<const PluginActionBatch::ActionStep> step) const;

    QUuid m_id;
    QSharedPointer<PluginActionBatch> m_batch;
    QList<QSharedPointer<BitContainer>> m_inputContainers;

    bool m_cancelled;
    bool m_running;
    QStringList m_errorList;

    QSet<QUuid> m_ranSteps;
    QHash<QUuid, QList<QSharedPointer<BitContainer>>> m_stepOutputs;
    QHash<QUuid, QPair<QUuid, QSharedPointer<AnalyzerRunner>>> m_analyzerRunners;
    QHash<QUuid, QPair<QUuid, QSharedPointer<OperatorRunner>>> m_operatorRunners;

    QHash<QSharedPointer<const PluginActionBatch::ActionStep>, QList<QPair<QUuid, int>>> m_trueStepInputs;

    QSharedPointer<PluginActionManager> m_actionManager;
};

#endif // BATCHRUNNER_H
