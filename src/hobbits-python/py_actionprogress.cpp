#include "py_actionprogress.h"
#include "pluginactionprogress.h"
#include <structmember.h>

#define UNUSED(expr) do { (void)(expr); } while (0);
#define PROGRESS(X) static_cast<PluginActionProgress*>(PyCapsule_GetPointer(X, nullptr))

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
    UNUSED(args)
    UNUSED(kwds)
    PyActionProgressObj *self;
    self = reinterpret_cast<PyActionProgressObj*>(type->tp_alloc(type, 0));
    if (self != nullptr) {
        self->progressCapsule = nullptr;
    }
    return (PyObject*) self;
}

static int PyActionProgress_init(PyActionProgressObj *self, PyObject *args, PyObject *kwds)
{
    UNUSED(kwds)
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
    PluginActionProgress* progress = PROGRESS(self->progressCapsule);
    return PyBool_FromLong(progress->isCancelled());
}

static PyObject* PyActionProgress_set_progress_percent(PyActionProgressObj *self, PyObject *args )
{
    int percent;
    if (!PyArg_ParseTuple(args, "i", &percent)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires an integer percent value");
        return nullptr;
    }

    PluginActionProgress* progress = PROGRESS(self->progressCapsule);
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

    PluginActionProgress* progress = PROGRESS(self->progressCapsule);
    progress->setProgress(current, max);
    Py_RETURN_NONE;
}

static PyMethodDef PyActionProgress_methods[] = {
    { "is_cancelled", PyCFunction(PyActionProgress_is_cancelled), METH_NOARGS, nullptr },
    { "set_progress_percent", PyCFunction(PyActionProgress_set_progress_percent), METH_VARARGS, nullptr },
    { "set_progress", PyCFunction(PyActionProgress_set_progress), METH_VARARGS, nullptr },
    {}  /* Sentinel */
};

static PyMemberDef PyActionProgress_members[] = {
    {}  /* Sentinel */
};

PyTypeObject PyActionProgress = {
    PyVarObject_HEAD_INIT(nullptr, 0)

    "hobbits.PluginActionProgress", // const char *tp_name; /* For printing, in format "<module>.<name>" */
    sizeof(PyActionProgressObj), // Py_ssize_t tp_basicsize, ; /* For allocation */
    0, //tp_itemsize

    /* Methods to implement standard operations */

    destructor(PyActionProgress_dealloc), // destructor tp_dealloc;
    0, // Py_ssize_t tp_vectorcall_offset;
    nullptr, // getattrfunc tp_getattr;
    nullptr, // setattrfunc tp_setattr;
    nullptr, // PyAsyncMethods *tp_as_async; /* formerly known as tp_compare (Python 2) or tp_reserved (Python 3) */
    nullptr, // reprfunc tp_repr;

    /* Method suites for standard classes */

    nullptr, // PyNumberMethods *tp_as_number;
    nullptr, // PySequenceMethods *tp_as_sequence;
    nullptr, // PyMappingMethods *tp_as_mapping;

    /* More standard operations (here for binary compatibility) */

    nullptr, // hashfunc tp_hash;
    nullptr, // ternaryfunc tp_call;
    nullptr, // reprfunc tp_str;
    nullptr, // getattrofunc tp_getattro;
    nullptr, // setattrofunc tp_setattro;

    /* Functions to access object as input/output buffer */
    nullptr, // PyBufferProcs *tp_as_buffer;

    /* Flags to define presence of optional/expanded features */
    Py_TPFLAGS_DEFAULT, // unsigned long tp_flags;

    "Hobbits Plugin Action Progress", // const char *tp_doc; /* Documentation string */

    /* Assigned meaning in release 2.0 */
    /* call function for all accessible objects */
    nullptr, // traverseproc tp_traverse;

    /* delete references to contained objects */
    nullptr, // inquiry tp_clear;

    /* Assigned meaning in release 2.1 */
    /* rich comparisons */
    nullptr, // richcmpfunc tp_richcompare;

    /* weak reference enabler */
    0, // Py_ssize_t tp_weaklistoffset;

    /* Iterators */
    nullptr, // getiterfunc tp_iter;
    nullptr, // iternextfunc tp_iternext;

    /* Attribute descriptor and subclassing stuff */
    PyActionProgress_methods, // struct PyMethodDef *tp_methods;
    PyActionProgress_members, // struct PyMemberDef *tp_members;
    nullptr, // struct PyGetSetDef *tp_getset;
    nullptr, // struct _typeobject *tp_base;
    nullptr, // PyObject *tp_dict;
    nullptr, // descrgetfunc tp_descr_get;
    nullptr, // descrsetfunc tp_descr_set;
    0, // Py_ssize_t tp_dictoffset;
    initproc(PyActionProgress_init), // initproc tp_init;
    nullptr, // allocfunc tp_alloc;
    PyActionProgress_new, // newfunc tp_new;
    nullptr, // freefunc tp_free; /* Low-level free-memory routine */
    nullptr, // inquiry tp_is_gc; /* For PyObject_IS_GC */
    nullptr, // PyObject *tp_bases;
    nullptr, // PyObject *tp_mro; /* method resolution order */
    nullptr, // PyObject *tp_cache;
    nullptr, // PyObject *tp_subclasses;
    nullptr, // PyObject *tp_weaklist;
    nullptr, // destructor tp_del;

    /* Type attribute cache version tag. Added in version 2.6 */
    0, // unsigned int tp_version_tag;

    nullptr, // destructor tp_finalize;
    nullptr, // vectorcallfunc tp_vectorcall;
    // Py_DEPRECATED(3.8) int (*tp_print)(PyObject *, FILE *, int);
};
