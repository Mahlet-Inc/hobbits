#include "batchrunner.h"

BatchRunner::BatchRunner() :
    m_id(QUuid::createUuid())
{

}

QSharedPointer<BatchRunner> BatchRunner::create(
        QSharedPointer<PluginActionBatch> batch,
        QList<QSharedPointer<BitContainer>> inputContainers)
{
    auto runner = QSharedPointer<BatchRunner>(new BatchRunner());
    runner->m_batch = batch;
    runner->m_inputContainers = inputContainers;


    for (auto step: batch->actionSteps()) {
        for (auto input: step->inputs) {
            if (input.first.isNull()) {
                QUuid specialInput = QUuid::createUuid();
            }
        }
    }

    return runner;
}

QUuid BatchRunner::id() const
{
    return m_id;
}

void BatchRunner::run()
{
    checkForRunnableSteps();
}

void BatchRunner::checkForRunnableSteps()
{
    for (auto step: m_batch->actionSteps()) {
        bool hasInputs = true;
        for (auto input: step->inputs) {
            if (input.first.isNull()) {
                continue;
            }
        }
    }
}
