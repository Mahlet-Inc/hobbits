#include "hobbitspython.h"
#include "pythoninterpreter.h"
#include <QtConcurrent/QtConcurrent>

HobbitsPython::HobbitsPython()
{
    m_pool.setMaxThreadCount(1);
    m_pool.setExpiryTimeout(-1);
}

HobbitsPython& HobbitsPython::getInstance()
{
    static HobbitsPython instance;
    return instance;
}

QSharedPointer<ActionWatcher<QSharedPointer<PythonResult> > > HobbitsPython::runProcessScript(
        QString scriptPath,
        QSharedPointer<const BitArray> inputBitArray,
        QSharedPointer<BitArray> outputBitArray,
        QSharedPointer<ActionProgress> progress)
{
    QMutexLocker lock(&m_mutex);

    if (progress.isNull()) {
        progress = QSharedPointer<ActionProgress>(new ActionProgress());
    }

    auto future = QtConcurrent::run(
            &m_pool,
            PythonInterpreter::runProcessScript,
            scriptPath,
            inputBitArray,
            outputBitArray,
            progress);

    QSharedPointer<ActionWatcher<QSharedPointer<PythonResult>>> watcher(
            new ActionWatcher<QSharedPointer<PythonResult>>(future, progress));

    return watcher;
}
