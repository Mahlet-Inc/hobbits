#ifndef HOBBITSPYTHON_H
#define HOBBITSPYTHON_H

#include <QThreadPool>
#include <QThread>
#include <QMutex>
#include "pythonresult.h"
#include "actionwatcher.h"
#include "bitarray.h"
#include "pythonrequest.h"

#include "hobbits-python_global.h"
class HOBBITSPYTHONSHARED_EXPORT HobbitsPython : public QObject
{
    Q_OBJECT

public:
    static HobbitsPython& getInstance();

    QSharedPointer<ActionWatcher<QSharedPointer<PythonResult>>> runProcessScript(
            QSharedPointer<PythonRequest> request,
            bool includeActionProgressArg = true);

    QSharedPointer<ActionWatcher<QSharedPointer<PythonResult>>> runProcessScript(
            QSharedPointer<PythonRequest> request,
            QSharedPointer<ActionProgress> progress);

    // copying dependencies from a resource into a temp folder might be used in many python run staging scenarios
    static void recursiveDirCopy(QString src, QString dest);

private:
    HobbitsPython();
};

#endif // HOBBITSPYTHON_H
