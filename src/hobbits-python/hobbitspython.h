#ifndef HOBBITSPYTHON_H
#define HOBBITSPYTHON_H

#include <QThreadPool>
#include <QThread>
#include <QMutex>
#include "pythonresult.h"
#include "actionwatcher.h"
#include "bitarray.h"

class PythonInterpreter;

#include "hobbits-python_global.h"
class HOBBITSPYTHONSHARED_EXPORT HobbitsPython : public QObject
{
    Q_OBJECT

public:
    static HobbitsPython& getInstance();

    QSharedPointer<ActionWatcher<QSharedPointer<PythonResult>>> runProcessScript(
            QString scriptPath,
            QSharedPointer<const BitArray> inputBitArray,
            QSharedPointer<BitArray> outputBitArray,
            QSharedPointer<ActionProgress> progress = QSharedPointer<ActionProgress>());

private:
    HobbitsPython();
    QMutex m_mutex;
    QThreadPool m_pool;
};

#endif // HOBBITSPYTHON_H
