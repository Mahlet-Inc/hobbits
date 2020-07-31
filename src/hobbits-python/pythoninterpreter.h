#ifndef PYTHONINTERPRETER_H
#define PYTHONINTERPRETER_H

#define PY_SSIZE_T_CLEAN
#include <python3.7m/Python.h>
#include <python3.7m/structmember.h>

#include "pythonresult.h"
#include "actionprogress.h"
#include "bitarray.h"

class PythonInterpreter
{
public:
    static QSharedPointer<PythonResult> runProcessScript(
            QString scriptPath,
            QSharedPointer<const BitArray> inputBitArray,
            QSharedPointer<BitArray> outputBitArray,
            QSharedPointer<ActionProgress> progress);


private:
    PythonInterpreter() {}

    static PyObject* callFunction(PyObject* module, const char* name, PyObject* args = nullptr);

    static PyObject* hobbitsTypeWrapper(PyObject* hobbitsModule, const char* typeName, void *toWrap);
    static PyObject* bitArray(PyObject* hobbitsModule, QSharedPointer<BitArray> bitArray);
    static PyObject* immutableBitArray(PyObject* hobbitsModule, QSharedPointer<const BitArray> bitArray);
    static PyObject* actionProgress(PyObject* hobbitsModule, QSharedPointer<ActionProgress> progress);

    static QSharedPointer<PythonResult> finalize(QFile &stdoutFile, QFile &stderrFile, QStringList errors = QStringList());
};

#endif // PYTHONINTERPRETER_H
