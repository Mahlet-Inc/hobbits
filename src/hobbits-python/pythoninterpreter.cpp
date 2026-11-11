#define PY_SSIZE_T_CLEAN
#include "pythonapi.h"
#include <structmember.h>

#include "pythoninterpreter.h"
#include <QTemporaryDir>
#include <QMutex>
#include <QFileInfo>
#include <QStandardPaths>
#include <QProcess>
#include "settingsmanager.h"
#include "py_hobbits.h"
#include <hobbitspythonconfig.h>

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

// Attach the interpreter thread state and hold the GIL while running a request.
class ScopedPythonLock {
public:
    ScopedPythonLock() : m_state(PyGILState_Ensure()) {}
    ~ScopedPythonLock() { PyGILState_Release(m_state); }
private:
    PyGILState_STATE m_state;
};

class ScopedPythonEnvironment {
public:
    ScopedPythonEnvironment() :
        m_stdout(PySys_GetObject("stdout")),
        m_stderr(PySys_GetObject("stderr")),
        m_path(PySequence_List(PySys_GetObject("path")))
    {
        Py_XINCREF(m_stdout);
        Py_XINCREF(m_stderr);
    }

    ~ScopedPythonEnvironment()
    {
        // Close only the streams opened for this request, then restore the host.
        for (const char *name : {"stdout", "stderr"}) {
            PyObject *stream = PySys_GetObject(name);
            if (stream && stream != m_stdout && stream != m_stderr) {
                ScopedPyObj closed(PyObject_CallMethod(stream, "close", nullptr));
                if (closed.isNull()) {
                    PyErr_Clear();
                }
            }
        }
        PySys_SetObject("stdout", m_stdout);
        PySys_SetObject("stderr", m_stderr);
        if (m_path) {
            PySys_SetObject("path", m_path);
        }
        PyDict_DelItemString(PyImport_GetModuleDict(), "thescript");
        PyErr_Clear();
        Py_XDECREF(m_stdout);
        Py_XDECREF(m_stderr);
        Py_XDECREF(m_path);
    }

    bool redirect(const char *name, const QString &path)
    {
        ScopedPyObj builtins(PyImport_ImportModule("builtins"));
        if (builtins.isNull()) {
            return false;
        }
        ScopedPyObj stream(PyObject_CallMethod(builtins.obj(), "open", "ssi", path.toUtf8().constData(), "w", 1));
        return !stream.isNull() && PySys_SetObject(name, stream.obj()) == 0;
    }

private:
    PyObject *m_stdout;
    PyObject *m_stderr;
    PyObject *m_path;
};

PythonInterpreter::PythonInterpreter() : m_worker(new QObject())
{
    // CPython must be finalized on the same native thread that initialized it.
    // A QtConcurrent pool thread can disappear before application shutdown.
    m_worker->moveToThread(&m_thread);
    QObject::connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
    m_thread.start();
    QMetaObject::invokeMethod(m_worker, [this] { initialize(); }, Qt::BlockingQueuedConnection);
}

PythonInterpreter::~PythonInterpreter()
{
    QMutexLocker lock(&m_mutex);
    QMetaObject::invokeMethod(m_worker, [this] {
        if (m_initializationError.isNull() && Py_IsInitialized()) {
            // Finalization consumes the thread state; do not release the GIL afterward.
            PyGILState_Ensure();
            Py_FinalizeEx();
        }
    }, Qt::BlockingQueuedConnection);
    m_thread.quit();
    m_thread.wait();
}

QSharedPointer<PythonResult> PythonInterpreter::runProcessScript(QSharedPointer<PythonRequest> request)
{
    auto &interpreter = instance();
    QMutexLocker lock(&interpreter.m_mutex);
    QSharedPointer<PythonResult> result;
    QMetaObject::invokeMethod(interpreter.m_worker, [&] {
        try {
            result = interpreter._runProcessScript(request);
        }
        catch (const std::exception &error) {
            result = PythonResult::result({QString("Python execution failed: %1").arg(error.what())});
        }
    }, Qt::BlockingQueuedConnection);
    return result;
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

    PyConfig config;
    PyConfig_InitPythonConfig(&config);
    config.parse_argv = 0;
    config.install_signal_handlers = 0;

    // Preserve the Python selected by CMake (including a virtual environment).
    // Matching interpreters on PATH allow a relocated installation to recover.
    QString programPath;
    const QString versionedName = QString("python%1.%2").arg(PY_MAJOR_VERSION).arg(PY_MINOR_VERSION);
    const QString configuredProgram = QString::fromUtf8(HobbitsPythonConfig::PYTHON_EXECUTABLE);
    const QStringList candidates = {configuredProgram,
                                   QStandardPaths::findExecutable(versionedName),
                                   QStandardPaths::findExecutable("python3"),
                                   QStandardPaths::findExecutable("python")};
    for (const QString &candidate : candidates) {
        if (candidate.isEmpty() || !QFileInfo::exists(candidate)) {
            continue;
        }
        QProcess probe;
        probe.start(candidate, {"-I", "-S", "-c", "import sys; print('%d.%d' % sys.version_info[:2])"});
        if (probe.waitForFinished(5000) && probe.exitCode() == 0 &&
                probe.readAllStandardOutput().trimmed() == QByteArray::number(PY_MAJOR_VERSION) + "." + QByteArray::number(PY_MINOR_VERSION)) {
            programPath = candidate;
            break;
        }
        probe.kill();
        probe.waitForFinished();
    }
    PyStatus status = PyStatus_Ok();
    if (!programPath.isEmpty()) {
        const std::wstring program = programPath.toStdWString();
        status = PyConfig_SetString(&config, &config.program_name, program.c_str());
    }
    const std::wstring home = SettingsManager::getTransientSetting(SettingsManager::PYTHON_HOME_KEY).toString().toStdWString();
    if (!PyStatus_Exception(status) && !home.empty()) {
        status = PyConfig_SetString(&config, &config.home, home.c_str());
    }
    if (!PyStatus_Exception(status)) {
        status = Py_InitializeFromConfig(&config);
    }
    if (PyStatus_Exception(status)) {
        m_initializationError = PythonResult::result({QString("Failed to initialize Python: %1")
            .arg(status.err_msg ? status.err_msg : "Python requested exit")});
    }
    PyConfig_Clear(&config);
    if (m_initializationError.isNull()) {
        // Release the initialization thread's GIL before dispatching worker jobs.
        PyEval_SaveThread();
    }
}

void PythonInterpreter::waitForLock()
{
    QMutexLocker lock(&(instance().m_mutex));
}

QSharedPointer<PythonResult> PythonInterpreter::_runProcessScript(QSharedPointer<PythonRequest> request)
{
    if (!m_initializationError.isNull()) {
        return m_initializationError;
    }

    ScopedPythonLock pythonLock;

    QStringList errors;
    QTemporaryDir temp(QDir::temp().absoluteFilePath("HobbitsPythonXXXXXX"));

    if (!temp.isValid()) {
        return PythonResult::result({"Failed to create a temporary Python script directory"});
    }

    ScopedPythonEnvironment environment;
    QFile stderrFile(temp.filePath("stderr.log"));
    QFile stdoutFile(temp.filePath("stdout.log"));
    if (!environment.redirect("stderr", stderrFile.fileName()) ||
            !environment.redirect("stdout", stdoutFile.fileName())) {
        errorCheckAndPrint();
        return PythonResult::result({"Failed to redirect Python output"});
    }

    if (!QFile::copy(request->scriptName(), temp.filePath("thescript.py"))) {
        return finish(stdoutFile, stderrFile, {"Failed to copy the Python script"});
    }

    PyObject *sysPath = PySys_GetObject("path");
    ScopedPyObj scriptPath(PyUnicode_FromString(temp.path().toUtf8().constData()));
    if (scriptPath.isNull() || PyList_Insert(sysPath, 0, scriptPath.obj()) != 0) {
        errorCheckAndPrint();
        return finish(stdoutFile, stderrFile, {"Failed to set the Python script path"});
    }
    for (const QString &extension : request->pathExtensions()) {
        ScopedPyObj path(PyUnicode_FromString(extension.toUtf8().constData()));
        if (path.isNull() || PyList_Append(sysPath, path.obj()) != 0) {
            errorCheckAndPrint();
            return finish(stdoutFile, stderrFile, {"Failed to extend the Python search path"});
        }
    }

    // Import hobbits module
    ScopedPyObj hobbitsModule(PyImport_ImportModule("hobbits"));
    if (hobbitsModule.isNull()) {
        errorCheckAndPrint();
        errors.append("Failed to import 'hobbits' Python module");
        return finish(stdoutFile, stderrFile, errors);
    }

    // Remove the previous module so the script executes exactly once per request.
    if (PyDict_GetItemString(PyImport_GetModuleDict(), "thescript")) {
        PyDict_DelItemString(PyImport_GetModuleDict(), "thescript");
    }
    ScopedPyObj scriptName(PyUnicode_DecodeFSDefault("thescript"));
    ScopedPyObj scriptModule(PyImport_Import(scriptName.obj()));
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
                    errors.append(QString("Failed to parse arg %1").arg(i+1));
                    return finish(stdoutFile, stderrFile, errors);
                }
                int setCode = PyTuple_SetItem(args.obj(), i, arg);
                if (setCode != 0) {
                    errorCheckAndPrint();
                    errors.append(QString("Failed to set arg %1 in position").arg(i+1));
                    return finish(stdoutFile, stderrFile, errors);
                }
            }
        }

        // Run the function
        ScopedPyObj result(callFunction(scriptModule.obj(), request->functionName().toStdString().c_str(), args.obj()));
        if (result.isNull()) {
            errorCheckAndPrint();
            errors.append(QString("Failed to call Python function '%1'").arg(request->functionName()));
        }
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
    if (type.isNull() || capsule.isNull()) {
        return nullptr;
    }
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
        // The wrapper already owns its reference; return it directly to the tuple.
        return obj;
    }
    else if (arg->type() == PythonArg::String) {
        return Py_BuildValue(arg->argSymbol().toStdString().c_str(), arg->stringData().toStdString().c_str());
    }
    else if (arg->type() == PythonArg::Integer) {
        return Py_BuildValue(arg->argSymbol().toStdString().c_str(), arg->integerData());
    }
    else if (arg->type() == PythonArg::Double) {
        return Py_BuildValue(arg->argSymbol().toStdString().c_str(), arg->doubleData());
    }
    else if (arg->type() == PythonArg::Boolean) {
        return Py_BuildValue(arg->argSymbol().toStdString().c_str(), arg->boolData() ? Py_True : Py_False);
    }
    else if (arg->type() == PythonArg::ByteBuffer) {
        return PyMemoryView_FromMemory(reinterpret_cast<char*>(arg->pointer()), arg->integerData(), PyBUF_WRITE);
    }
    else if (arg->type() == PythonArg::ImageBuffer) {
        unsigned int width = uint(arg->sizeData().width());
        unsigned int height = uint(arg->sizeData().height());
        ScopedPyObj memview(PyMemoryView_FromMemory(reinterpret_cast<char*>(arg->pointer()), width * height * 4, PyBUF_WRITE));
        ScopedPyObj type(PyObject_GetAttrString(hobbitsModule, "ImageBuffer"));
        if (memview.isNull() || type.isNull()) {
            return nullptr;
        }
        return PyObject_CallFunction(type.obj(), "OII", memview.obj(), width, height);
    }
    else {
        return nullptr;
    }
}

QSharedPointer<PythonResult> finish(QFile &stdoutFile, QFile &stderrFile, QStringList errors)
{
    for (const char *name : {"stderr", "stdout"}) {
        PyObject *stream = PySys_GetObject(name);
        if (stream) {
            ScopedPyObj flushed(PyObject_CallMethod(stream, "flush", nullptr));
            if (flushed.isNull()) {
                PyErr_Clear();
            }
        }
    }

    return PythonResult::result(stdoutFile, stderrFile, errors);
}
