#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "pythoninterpreter.h"
#include <QTemporaryDir>
#include <QMutex>
#include "settingsmanager.h"
#include "py_hobbits.h"

PyObject* callFunction(PyObject* module, const char* name, PyObject* args = nullptr);
PyObject* hobbitsTypeWrapper(PyObject* hobbitsModule, const char* typeName, void *toWrap);
PyObject* parseArg(PyObject *hobbitsModule, PythonArg *arg);
QSharedPointer<PythonResult> finish(QFile &stdoutFile, QFile &stderrFile, QStringList errors = QStringList());

class ScopedPyObj {
public:
    ScopedPyObj(PyObject* obj = nullptr) :
        m_obj(obj) {}

    ~ScopedPyObj() { Py_XDECREF(m_obj); }

    ScopedPyObj(ScopedPyObj const&) = delete;
    void operator=(ScopedPyObj const&) = delete;

    ScopedPyObj& operator=(PyObject *obj) {
        Py_XDECREF(m_obj);
        m_obj = obj;
        return *this;
    }

    PyObject* obj() const { return m_obj; }
    bool isNull() const { return m_obj == nullptr; }

private:
    PyObject *m_obj;
};

PythonInterpreter::PythonInterpreter()
{
    initialize();
}

PythonInterpreter::~PythonInterpreter()
{
    if (m_initializationError.isNull()) {
        Py_FinalizeEx();
    }
}

QSharedPointer<PythonResult> PythonInterpreter::runProcessScript(QSharedPointer<PythonRequest> request)
{
    return instance()._runProcessScript(request);
}

PythonInterpreter &PythonInterpreter::instance()
{
    static PythonInterpreter instance;
    return instance;
}

void PythonInterpreter::initialize()
{
    if (PyImport_AppendInittab("hobbits", &PyInit_hobbits) == -1) {
        m_initializationError = PythonResult::result({"Failed PyImport_AppendInittab with 'hobbits' module"});
        return;
    }

    PyStatus status;
    PyConfig config;
    PyConfig_InitPythonConfig(&config);

    std::wstring wsPyHome = SettingsManager::getInstance().getTransientSetting(SettingsData::PYTHON_HOME_KEY).toString().toStdWString();
    QScopedPointer<wchar_t> pyHome(new wchar_t[wsPyHome.length() + 1]);
    wcscpy(pyHome.data(), wsPyHome.c_str());
    config.home = pyHome.data();

    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        m_initializationError = PythonResult::result({QString("Failed Py_InitializeFromConfig - is there a valid python at '%1'?").arg(QString::fromStdWString(wsPyHome))});
        return;
    }
}

QSharedPointer<PythonResult> PythonInterpreter::_runProcessScript(QSharedPointer<PythonRequest> request)
{
    if (!m_initializationError.isNull()) {
        return m_initializationError;
    }

    QMutexLocker lock(&m_mutex);

    QStringList errors;
    QTemporaryDir temp(QDir::temp().absoluteFilePath("HobbitsPythonXXXXXX"));

    // Setup stderr and stdout
    QFile stderrFile(temp.filePath("stderr.log"));
    QFile stdoutFile(temp.filePath("stdout.log"));
    PyRun_SimpleString("import sys");
    PyRun_SimpleString(QString("sys.stderr = open('%1', 'w', buffering=1 )").arg(stderrFile.fileName()).toStdString().c_str());
    PyRun_SimpleString(QString("sys.stdout = open('%1', 'w', buffering=1 )").arg(stdoutFile.fileName()).toStdString().c_str());

    // Get the script in sys.path
    QFile::copy(request->scriptName(), temp.filePath("thescript.py"));
    PyRun_SimpleString(QString("sys.path.append('%1')").arg(temp.path()).toStdString().c_str());

    // Append any other sys.path extensions
    for (QString extension : request->pathExtensions()) {
        PyRun_SimpleString(QString("sys.path.append('%1')").arg(extension).toStdString().c_str());
    }

    // Import hobbits module
    ScopedPyObj hobbitsModule(PyImport_ImportModule("hobbits"));
    if (hobbitsModule.isNull()) {
        errorCheckAndPrint();
        errors.append("Failed to import 'hobbits' Python module");
        return finish(stdoutFile, stderrFile, errors);
    }

    // Import custom script, reloading to make sure it's not using 'thescript' from the previous run
    ScopedPyObj scriptName(PyUnicode_DecodeFSDefault("thescript"));
    ScopedPyObj scriptModule(PyImport_Import(scriptName.obj()));
    if (scriptModule.isNull()) {
        errorCheckAndPrint();
        errors.append("Failed to import custom Python script");
        return finish(stdoutFile, stderrFile, errors);
    }
    scriptModule = PyImport_ReloadModule(scriptModule.obj());
    if (scriptModule.isNull()) {
        errorCheckAndPrint();
        errors.append("Failed to import custom Python script");
        return finish(stdoutFile, stderrFile, errors);
    }

    // Run a function in the script, if specified
    if (!request->functionName().isEmpty()) {
        // Collect and build the arguments
        ScopedPyObj args;
        if (request->args().size() > 0) {
            args = PyTuple_New(request->args().size());
            if (args.isNull()) {
                errors.append("Failed to create a tuple for function arguments");
                return finish(stdoutFile, stderrFile, errors);
            }
            for (int i = 0; i < request->args().size(); i++) {
                PyObject *arg = parseArg(hobbitsModule.obj(), request->args().at(i));
                if (arg == nullptr) {
                    errors.append(QString("Failed to parse arg %1").arg(i));
                    return finish(stdoutFile, stderrFile, errors);
                }
                int setCode = PyTuple_SetItem(args.obj(), i, arg);
                if (setCode != 0) {
                    errorCheckAndPrint();
                    errors.append(QString("Failed to set arg %1 in position").arg(i));
                    return finish(stdoutFile, stderrFile, errors);
                }
            }
        }

        // Run the function
        ScopedPyObj result(callFunction(scriptModule.obj(), request->functionName().toStdString().c_str(), args.obj()));
        errorCheckAndPrint();
    }

    return finish(stdoutFile, stderrFile, errors);
}

bool PythonInterpreter::errorCheckAndPrint()
{
    if (PyErr_Occurred()) {
        PyErr_Print();
        return true;
    }
    return false;
}

PyObject* callFunction(PyObject *module, const char *name, PyObject *args)
{
    ScopedPyObj func(PyObject_GetAttrString(module, name));
    PyObject *result = nullptr;

    if (!func.isNull() && PyCallable_Check(func.obj())) {
        result = PyObject_CallObject(func.obj(), args);
    }
    else if (PyErr_Occurred()) {
        PyErr_Print();
    }

    return result;
}

PyObject* hobbitsTypeWrapper(PyObject *hobbitsModule, const char *typeName, void *toWrap)
{
    ScopedPyObj type(PyObject_GetAttrString(hobbitsModule, typeName));
    ScopedPyObj capsule(PyCapsule_New(toWrap, nullptr, nullptr));
    PyObject* instance = PyObject_CallFunction(type.obj(), "O", capsule.obj());
    return instance;
}

PyObject* parseArg(PyObject *hobbitsModule, PythonArg *arg)
{
    if (arg->type() == PythonArg::HobbitsWrapper) {
        auto obj = hobbitsTypeWrapper(hobbitsModule, arg->wrapType().toStdString().c_str(), arg->pointer());
        if (!obj) {
            return nullptr;
        }
        return Py_BuildValue(arg->argSymbol().toStdString().c_str(), obj);
    }
    else if (arg->type() == PythonArg::String) {
        return Py_BuildValue(arg->argSymbol().toStdString().c_str(), arg->stringData().toStdString().c_str());
    }
    else if (arg->type() == PythonArg::Integer) {
        return Py_BuildValue(arg->argSymbol().toStdString().c_str(), arg->integerData());
    }
    else {
        return nullptr;
    }
}

QSharedPointer<PythonResult> finish(QFile &stdoutFile, QFile &stderrFile, QStringList errors)
{
    PyRun_SimpleString("sys.stderr.close()");
    PyRun_SimpleString("sys.stdout.close()");

    return PythonResult::result(stdoutFile, stderrFile, errors);
}
