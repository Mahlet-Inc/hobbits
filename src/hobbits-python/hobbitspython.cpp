#include "hobbitspython.h"
#include "pythoninterpreter.h"
#include <QtConcurrent/QtConcurrent>

HobbitsPython::HobbitsPython()
{
}

HobbitsPython& HobbitsPython::getInstance()
{
    static HobbitsPython instance;
    return instance;
}

QSharedPointer<ActionWatcher<QSharedPointer<PythonResult> > > HobbitsPython::runProcessScript(
        QSharedPointer<PythonRequest> request,
        bool includeActionProgressArg)
{
    if (includeActionProgressArg) {
        return runProcessScript(request, QSharedPointer<ActionProgress>(new ActionProgress()));
    }
    else {
        return runProcessScript(request, QSharedPointer<ActionProgress>());
    }
}

QSharedPointer<ActionWatcher<QSharedPointer<PythonResult> > > HobbitsPython::runProcessScript(QSharedPointer<PythonRequest> request, QSharedPointer<ActionProgress> progress)
{
    if (progress.isNull()) {
        progress = QSharedPointer<ActionProgress>(new ActionProgress());
    }
    else {
        request->addArg(PythonArg::actionProgress(progress));
    }

    auto future = QtConcurrent::run(
            QThreadPool::globalInstance(),
            PythonInterpreter::runProcessScript,
            request);

    QSharedPointer<ActionWatcher<QSharedPointer<PythonResult>>> watcher(
            new ActionWatcher<QSharedPointer<PythonResult>>(future, progress));

    return watcher;
}
