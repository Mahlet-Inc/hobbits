#include "pythoninterpreter.h"
#include <QTemporaryDir>

#include "py_hobbits.h"

static bool hobbitsAppended;
QSharedPointer<PythonResult> PythonInterpreter::runProcessScript(QString scriptPath, QSharedPointer<const BitArray> inputBitArray, QSharedPointer<BitArray> outputBitArray, QSharedPointer<ActionProgress> progress)
{
    QTemporaryDir temp("HobbitsPythonXXXXXX");

    QFile stderrFile(temp.filePath("stderr.log"));
    QFile stdoutFile(temp.filePath("stdout.log"));
    QStringList errors;

    QFile::copy(scriptPath, temp.filePath("thescript.py"));

    if (!hobbitsAppended) {
        if (PyImport_AppendInittab("hobbits", &PyInit_hobbits) == -1) {
            return PythonResult::result(errors);
        }
        hobbitsAppended = true;
    }

    Py_InitializeEx(0);

    // Setup path and output files
    PyRun_SimpleString("import sys");
    PyRun_SimpleString(QString("sys.stderr = open('%1', 'w', buffering=1 )").arg(stderrFile.fileName()).toStdString().c_str());
    PyRun_SimpleString(QString("sys.stdout = open('%1', 'w', buffering=1 )").arg(stdoutFile.fileName()).toStdString().c_str());
    PyRun_SimpleString(QString("sys.path.append('%1')").arg(temp.path()).toStdString().c_str());

    // Import hobbits module
    PyObject* hobbitsModule = PyImport_ImportModule("hobbits");
    if (hobbitsModule == nullptr) {
        PyErr_Print();
        errors.append("Failed to import 'hobbits' Python module");
        return finalize(stdoutFile, stderrFile, errors);
    }

    // Import custom script
    PyObject *scriptName = PyUnicode_DecodeFSDefault("thescript");
    PyObject *scriptModule = PyImport_Import(scriptName);
    Py_DECREF(scriptName);

    if (scriptModule == nullptr) {
        PyErr_Print();
        errors.append("Failed to import custom Python script");
        Py_XDECREF(hobbitsModule);
        return finalize(stdoutFile, stderrFile, errors);
    }

    // Run the process_bits method in the custom script
    PyObject *inBits = immutableBitArray(hobbitsModule, inputBitArray);
    PyObject *outBits = bitArray(hobbitsModule, outputBitArray);
    PyObject *aProg = actionProgress(hobbitsModule, progress);
    PyObject *args = Py_BuildValue("OOO", inBits, outBits, aProg);
    PyObject *result = callFunction(scriptModule, "process_bits", args);
    Py_XDECREF(result);
    Py_XDECREF(args);
    Py_XDECREF(inBits);
    Py_XDECREF(outBits);

    // Cleanup
    Py_XDECREF(hobbitsModule);
    Py_XDECREF(scriptModule);

    return finalize(stdoutFile, stderrFile, errors);
}

PyObject* PythonInterpreter::callFunction(PyObject *module, const char *name, PyObject *args)
{
    PyObject* func = PyObject_GetAttrString(module, name);
    PyObject *result = nullptr;

    if (func && PyCallable_Check(func)) {
        result = PyObject_CallObject(func, args);
    }
    else {
        if (PyErr_Occurred())
            PyErr_Print();
    }

    Py_XDECREF(func);
    return result;
}

PyObject *PythonInterpreter::hobbitsTypeWrapper(PyObject *hobbitsModule, const char *typeName, void *toWrap)
{
    PyObject* type = PyObject_GetAttrString(hobbitsModule, typeName);
    PyObject* capsule = PyCapsule_New(toWrap, nullptr, nullptr);
    PyObject* instance = PyObject_CallFunction(type, "O", capsule);
    Py_DECREF(type);
    Py_DECREF(capsule);
    return instance;
}

PyObject *PythonInterpreter::bitArray(PyObject *hobbitsModule, QSharedPointer<BitArray> bitArray)
{
    return hobbitsTypeWrapper(hobbitsModule, "BitArray", static_cast<void*>(bitArray.data()));
}

PyObject *PythonInterpreter::immutableBitArray(PyObject *hobbitsModule, QSharedPointer<const BitArray> bitArray)
{
    return hobbitsTypeWrapper(hobbitsModule, "ImmutableBitArray", const_cast<void*>(static_cast<const void*>(bitArray.data())));
}

PyObject *PythonInterpreter::actionProgress(PyObject *hobbitsModule, QSharedPointer<ActionProgress> progress)
{
    return hobbitsTypeWrapper(hobbitsModule, "ActionProgress", static_cast<void*>(progress.data()));
}

QSharedPointer<PythonResult> PythonInterpreter::finalize(QFile &stdoutFile, QFile &stderrFile, QStringList errors)
{
    if (Py_FinalizeEx() != 0) {
        errors.append("Error encountered in Py_FinalizeEx()");
    }
    return PythonResult::result(stdoutFile, stderrFile, errors);
}
