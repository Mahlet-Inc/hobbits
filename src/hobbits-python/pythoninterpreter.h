#ifndef PYTHONINTERPRETER_H
#define PYTHONINTERPRETER_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "pythonrequest.h"
#include "pythonresult.h"
#include "actionprogress.h"
#include "bitarray.h"
#include "pythonarg.h"

class PythonInterpreter
{
public:
    static QSharedPointer<PythonResult> runProcessScript(QSharedPointer<PythonRequest> request);

private:
    PythonInterpreter() {}

    static PyObject* callFunction(PyObject* module, const char* name, PyObject* args = nullptr);

    static PyObject* hobbitsTypeWrapper(PyObject* hobbitsModule, const char* typeName, void *toWrap);
    static PyObject* parseArg(PyObject *hobbitsModule, PythonArg *arg);

    static QSharedPointer<PythonResult> finalize(QFile &stdoutFile, QFile &stderrFile, QStringList errors = QStringList());
};

#endif // PYTHONINTERPRETER_H
