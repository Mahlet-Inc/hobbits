#include "embeddedpythoninterpreter.h"
#include "hobbitsexecutioncontext.h"

EmbeddedPythonInterpreter::EmbeddedPythonInterpreter() :
    m_workingDir(QDir::tempPath() + "/HobbitsPythonXXXXXX")
{
    m_errorFile = m_workingDir.filePath("error.log");
}

void EmbeddedPythonInterpreter::runScript(QString scriptFileName, QSharedPointer<const BitArray> inputBitArray, QSharedPointer<BitArray> outputBitArray)
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;

    QFile::copy(scriptFileName, m_workingDir.filePath("thescript.py"));

    inBits = inputBitArray;
    outBits = outputBitArray;
    if (PyImport_AppendInittab("hobbitsexecutioncontext", &PyInit_hobbitsexecutioncontext) == -1) {
        fprintf(stderr, "Failed to extend built in modules with hobbitsexecutioncontext");
        return;
    }
    Py_Initialize();

    PyRun_SimpleString("import sys");
    PyRun_SimpleString(QString("sys.stderr = open('%1', 'w', buffering=1 )").arg(m_errorFile).toStdString().c_str());

    PyRun_SimpleString("import os");
    PyRun_SimpleString(QString("sys.path.append('%1')").arg(m_workingDir.path()).toStdString().c_str());

    pName = PyUnicode_DecodeFSDefault("thescript");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != nullptr) {
        pFunc = PyObject_GetAttrString(pModule, "process_bits");
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(0);
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != nullptr) {
                Py_DECREF(pValue);
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            writeError(QString("Cannot find function \"%1\"").arg("process_bits"));
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        writeError(QString("Failed to load \"%1\"").arg("thescript"));
        return;
    }
    if (Py_FinalizeEx() < 0) {
        return;
    }
}

QString EmbeddedPythonInterpreter::readErrors()
{
    QFile file(m_errorFile);
    if (file.open(QFile::ReadOnly)) {
        return file.readAll();
    }
    return QString();
}

void EmbeddedPythonInterpreter::writeError(QString error)
{
    QFile file(m_errorFile);
    if (file.open(QFile::Append)) {
        file.write(error.toUtf8());
        file.write("\n");
    }
}
