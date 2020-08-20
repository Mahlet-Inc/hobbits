#ifndef PYTHONINTERPRETER_H
#define PYTHONINTERPRETER_H

#include "pythonrequest.h"
#include "pythonresult.h"

class HOBBITSPYTHONSHARED_EXPORT PythonInterpreter
{
public:
    static QSharedPointer<PythonResult> runProcessScript(QSharedPointer<PythonRequest> request);

private:
    PythonInterpreter() {}
};

#endif // PYTHONINTERPRETER_H
