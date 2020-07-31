#include "py_actionprogress.h"
#include "actionprogress.h"

#include <python3.7m/structmember.h>

#define PROGRESS(X) static_cast<ActionProgress*>(PyCapsule_GetPointer(X, nullptr))

typedef struct {
    PyObject_HEAD
    PyObject* progressCapsule;
} PyActionProgressObj;

static void PyActionProgress_dealloc(PyActionProgressObj *self)
{
    Py_XDECREF(self->progressCapsule);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* PyActionProgress_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyActionProgressObj *self;
    self = reinterpret_cast<PyActionProgressObj*>(type->tp_alloc(type, 0));
    if (self != nullptr) {
        self->progressCapsule = nullptr;
    }
    return (PyObject*) self;
}

static int PyActionProgress_init(PyActionProgressObj *self, PyObject *args, PyObject *kwds)
{
    PyObject *progressCapsule;
    if (!PyArg_ParseTuple(args, "O", &progressCapsule)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a bit array capsule");
        return -1;
    }

    Py_INCREF(progressCapsule);
    self->progressCapsule = progressCapsule;
    return 0;
}

static PyObject* PyActionProgress_is_cancelled(PyActionProgressObj *self, PyObject *Py_UNUSED(ignored))
{
    ActionProgress* progress = PROGRESS(self->progressCapsule);
    return PyBool_FromLong(progress->getCancelled());
}

static PyObject* PyActionProgress_set_progress_percent(PyActionProgressObj *self, PyObject *args )
{
    int percent;
    if (!PyArg_ParseTuple(args, "i", &percent)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires an integer percent value");
        return nullptr;
    }

    ActionProgress* progress = PROGRESS(self->progressCapsule);
    progress->setProgressPercent(percent);
    Py_RETURN_NONE;
}

static PyObject* PyActionProgress_set_progress(PyActionProgressObj *self, PyObject *args )
{
    double current;
    double max;
    if (!PyArg_ParseTuple(args, "dd", &current, &max)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires current and max number values");
        return nullptr;
    }

    ActionProgress* progress = PROGRESS(self->progressCapsule);
    progress->setProgress(current, max);
    Py_RETURN_NONE;
}

static PyMethodDef PyActionProgress_methods[] = {
    { "is_cancelled", PyCFunction(PyActionProgress_is_cancelled), METH_NOARGS, nullptr },
    { "set_progress_percent", PyCFunction(PyActionProgress_set_progress_percent), METH_VARARGS, nullptr },
    { "set_progress", PyCFunction(PyActionProgress_set_progress), METH_VARARGS, nullptr },
    {NULL}  /* Sentinel */
};

static PyMemberDef PyActionProgress_members[] = {
    {nullptr}  /* Sentinel */
};

extern PyTypeObject PyActionProgress = {
    PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "hobbits.BitArray",
    .tp_basicsize = sizeof(PyActionProgressObj),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor) PyActionProgress_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "Hobbits Bit Array",
    .tp_methods = PyActionProgress_methods,
    .tp_members = PyActionProgress_members,
    .tp_init = (initproc) PyActionProgress_init,
    .tp_new = PyActionProgress_new,
};
