#ifndef HOBBITSEXECUTIONCONTEXT_H
#define HOBBITSEXECUTIONCONTEXT_H

#ifndef MYMODULE_H
#define MYMODULE_H

#define PY_SSIZE_T_CLEAN
#include <python3.7m/Python.h>

#include "bitarray.h"
#include <QSharedPointer>

extern "C" {

static QSharedPointer<const BitArray> inBits;
static QSharedPointer<BitArray> outBits;

static PyObject *inSizeInBits( PyObject *self )
{
    return PyLong_FromLong(inBits->sizeInBits());
}

static PyObject *outSizeInBits( PyObject *self )
{
    return PyLong_FromLong(outBits->sizeInBits());
}

static PyObject *inBitAt( PyObject *self, PyObject *args )
{
    long idx;
    if (!PyArg_ParseTuple(args, "l", &idx)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a bit index");
        PyErr_Print();
        return nullptr;
    }
    return PyBool_FromLong(inBits->at(idx));
}

static PyObject *outBitAt( PyObject *self, PyObject *args )
{
    long idx;
    if (!PyArg_ParseTuple(args, "l", &idx)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a bit index");
        PyErr_Print();
        return nullptr;
    }
    return PyBool_FromLong(outBits->at(idx));
}

static PyObject *resizeOutBits( PyObject *self, PyObject *args)
{
    long size;
    if (!PyArg_ParseTuple(args, "l", &size)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a size in bits");
        PyErr_Print();
        return nullptr;
    }
    outBits->resize(size);
    Py_RETURN_NONE;
}

static PyObject *setOutBitAt( PyObject *self, PyObject *args)
{
    long idx;
    int val;
    if (!PyArg_ParseTuple(args, "lp", &idx, &val)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires an index integer and a boolean bit value");
        PyErr_Print();
        return nullptr;
    }
    outBits->set(idx, val);
    Py_RETURN_NONE;
}

static PyMethodDef moduleMethods[] = {
    { "in_size", PyCFunction(inSizeInBits), METH_NOARGS, nullptr },
    { "out_size", PyCFunction(outSizeInBits), METH_NOARGS, nullptr },
    { "in_at", PyCFunction(inBitAt), METH_VARARGS, nullptr },
    { "out_at", PyCFunction(outBitAt), METH_VARARGS, nullptr },
    { "resize_out", PyCFunction(resizeOutBits), METH_VARARGS, nullptr },
    { "set_out", PyCFunction(setOutBitAt), METH_VARARGS, nullptr },
    { nullptr, nullptr, 0, nullptr }
};

static struct PyModuleDef HobbitsExecutionContextModule = {
    PyModuleDef_HEAD_INIT,
    "hobbitsexecutioncontext",   /* name of module */
    nullptr, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    moduleMethods
};

PyMODINIT_FUNC
PyInit_hobbitsexecutioncontext(void)
{
    return PyModule_Create(&HobbitsExecutionContextModule);
}

}

#endif // MYMODULE_H

#endif // HOBBITSEXECUTIONCONTEXT_H
