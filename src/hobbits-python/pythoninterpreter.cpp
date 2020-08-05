#include "pythoninterpreter.h"
#include <QTemporaryDir>
#include <QMutex>

#include "py_hobbits.h"

static QMutex mutex;
static bool hobbitsAppended;
QSharedPointer<PythonResult> PythonInterpreter::runProcessScript(QSharedPointer<PythonRequest> request)
{
    QMutexLocker lock(&mutex);

    QTemporaryDir temp("HobbitsPythonXXXXXX");

    QFile stderrFile(temp.filePath("stderr.log"));
    QFile stdoutFile(temp.filePath("stdout.log"));
    QStringList errors;

    QFile::copy(request->scriptName(), temp.filePath("thescript.py"));

    if (!hobbitsAppended) {
        if (PyImport_AppendInittab("hobbits", &PyInit_hobbits) == -1) {
            errors.append("Failed PyImport_AppendInittab with 'hobbits' module");
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

    for (QString extension : request->pathExtensions()) {
        PyRun_SimpleString(QString("sys.path.append('%1')").arg(extension).toStdString().c_str());
    }

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

    if (!request->functionName().isEmpty()) {
        // Collect and build the arguments
        PyObject *args = nullptr;
        if (request->args().size() > 0) {
            args = PyTuple_New(request->args().size());
            if (args == nullptr) {
                errors.append("Failed to create a tuple for function arguments");
                Py_XDECREF(hobbitsModule);
                Py_XDECREF(scriptModule);
                return finalize(stdoutFile, stderrFile, errors);
            }
            for (int i = 0; i < request->args().size(); i++) {
                PyObject* arg = parseArg(hobbitsModule, request->args().at(i));
                if (arg == nullptr) {
                    errors.append(QString("Failed to parse arg %1").arg(i));
                    Py_XDECREF(args);
                    Py_XDECREF(hobbitsModule);
                    Py_XDECREF(scriptModule);
                    return finalize(stdoutFile, stderrFile, errors);
                }
                int setCode = PyTuple_SetItem(args, i, arg);
                if (setCode != 0) {
                    PyErr_Print();
                    errors.append(QString("Failed to set arg %1 in position").arg(i));
                    Py_XDECREF(args);
                    Py_XDECREF(hobbitsModule);
                    Py_XDECREF(scriptModule);
                    return finalize(stdoutFile, stderrFile, errors);
                }
            }
        }

        // Run the function
        PyObject *result = callFunction(scriptModule, request->functionName().toStdString().c_str(), args);
        Py_XDECREF(result);
        Py_XDECREF(args);
    }

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

PyObject *PythonInterpreter::parseArg(PyObject *hobbitsModule, PythonArg *arg)
{
    if (arg->m_type == PythonArg::HobbitsWrapper) {
        auto obj = hobbitsTypeWrapper(hobbitsModule, arg->m_wrapType.toStdString().c_str(), arg->m_pointer);
        if (!obj) {
            return nullptr;
        }
        return Py_BuildValue(arg->m_argSymbol.toStdString().c_str(), obj);
    }
    else if (arg->m_type == PythonArg::String) {
        return Py_BuildValue(arg->m_argSymbol.toStdString().c_str(), arg->m_stringData.toStdString().c_str());
    }
    else {
        return nullptr;
    }
}

QSharedPointer<PythonResult> PythonInterpreter::finalize(QFile &stdoutFile, QFile &stderrFile, QStringList errors)
{
    if (Py_FinalizeEx() != 0) {
        errors.append("Error encountered in Py_FinalizeEx()");
    }
    return PythonResult::result(stdoutFile, stderrFile, errors);
}
