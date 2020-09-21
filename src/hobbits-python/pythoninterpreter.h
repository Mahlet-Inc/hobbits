#ifndef PYTHONINTERPRETER_H
#define PYTHONINTERPRETER_H

#include "pythonrequest.h"
#include "pythonresult.h"

class HOBBITSPYTHONSHARED_EXPORT PythonInterpreter
{
private:
    PythonInterpreter();

public:
    ~PythonInterpreter();

private:
    static PythonInterpreter& instance();
    void initialize();

public:
    static QSharedPointer<PythonResult> runProcessScript(QSharedPointer<PythonRequest> request);

private:
    QSharedPointer<PythonResult> _runProcessScript(QSharedPointer<PythonRequest> request);

    bool errorCheckAndPrint();

    QMutex m_mutex;
    QSharedPointer<PythonResult> m_initializationError;
};

#endif // PYTHONINTERPRETER_H
