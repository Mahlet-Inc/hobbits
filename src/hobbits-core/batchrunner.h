#ifndef BATCHRUNNER_H
#define BATCHRUNNER_H

#include <QObject>
#include "pluginactionbatch.h"

class BatchRunner : public QObject
{
    Q_OBJECT
public:
    static QSharedPointer<BatchRunner> create(QSharedPointer<PluginActionBatch>, QList<QSharedPointer<BitContainer>>);

    QUuid id() const;

    void run();

signals:
    void finished(QUuid);

private slots:
    void checkForRunnableSteps();

private:
    explicit BatchRunner();

    QUuid m_id;
    QSharedPointer<PluginActionBatch> m_batch;
    QList<QSharedPointer<BitContainer>> m_inputContainers;

    QHash<QUuid, QList<QSharedPointer<BitContainer>>> m_stepOutputs;
};

#endif // BATCHRUNNER_H
