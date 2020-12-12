#include "py_displayhandle.h"
#include "displayhandle.h"
#include <QSharedPointer>

#include <structmember.h>

#define UNUSED(expr) do { (void)(expr); } while (0);
#define HANDLE(X) static_cast<DisplayHandle*>(PyCapsule_GetPointer(X, nullptr))

typedef struct {
    PyObject_HEAD
    PyObject* handleCapsule;
} DisplayHandlePyObj;

static void PyDisplayHandle_dealloc(DisplayHandlePyObj *self)
{
    Py_XDECREF(self->handleCapsule);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* PyDisplayHandle_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    UNUSED(args)
    UNUSED(kwds)
    DisplayHandlePyObj *self;
    self = reinterpret_cast<DisplayHandlePyObj*>(type->tp_alloc(type, 0));
    if (self != nullptr) {
        self->handleCapsule = nullptr;
    }
    return (PyObject*) self;
}

static int PyDisplayHandle_init(DisplayHandlePyObj *self, PyObject *args, PyObject *kwds)
{
    UNUSED(kwds)
    PyObject *bitArrayCapsule;
    if (!PyArg_ParseTuple(args, "O", &bitArrayCapsule)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a bit array capsule");
        return -1;
    }

    Py_INCREF(bitArrayCapsule);
    self->handleCapsule = bitArrayCapsule;
    return 0;
}

static PyObject* PyDisplayHandle_frame_offset(DisplayHandlePyObj *self, void *Py_UNUSED(closure))
{
    DisplayHandle* handle = HANDLE(self->handleCapsule);
    return PyLong_FromLong(handle->frameOffset());
}

static PyObject* PyDisplayHandle_bit_offset(DisplayHandlePyObj *self, void *Py_UNUSED(closure))
{
    DisplayHandle* handle = HANDLE(self->handleCapsule);
    return PyLong_FromLong(handle->frameOffset());
}

static PyObject* PyDisplayHandle_total_bit_offset(DisplayHandlePyObj *self, void *Py_UNUSED(closure))
{
    DisplayHandle* handle = HANDLE(self->handleCapsule);
    if (handle->currentContainer().isNull()
            || handle->currentContainer()->frameCount() <= handle->frameOffset()) {
        return PyLong_FromLong(0);
    }

    return PyLong_FromLong(handle->currentContainer()->frameAt(handle->frameOffset()).start() + handle->bitOffset());
}

static PyObject* PyDisplayHandle_current_container(DisplayHandlePyObj *self, void *Py_UNUSED(closure))
{
    DisplayHandle* handle = HANDLE(self->handleCapsule);

    if (handle->currentContainer().isNull()) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    PyObject* hobbitsModuleName = PyUnicode_FromString("hobbits");
    PyObject* hobbitsModule = PyImport_GetModule(hobbitsModuleName);
    Py_XDECREF(hobbitsModuleName);
    PyObject* type = PyObject_GetAttrString(hobbitsModule, "ImmutableBitContainer");
    PyObject* capsule = PyCapsule_New(handle->currentContainer().data(), nullptr, nullptr);
    PyObject* container = PyObject_CallFunction(type, "O", capsule);
    Py_DECREF(type);
    Py_DECREF(capsule);

    return container;
}

static PyMethodDef PyDisplayHandle_methods[] = {
    {}  /* Sentinel */
};

static PyMemberDef PyDisplayHandle_members[] = {
    {}  /* Sentinel */
};

static PyGetSetDef PyDisplayHandle_getsets[] = {
    {"bit_offset", getter(PyDisplayHandle_bit_offset), nullptr, nullptr, nullptr},
    {"frame_offset", getter(PyDisplayHandle_frame_offset), nullptr, nullptr, nullptr},
    {"total_bit_offset", getter(PyDisplayHandle_total_bit_offset), nullptr, nullptr, nullptr},
    {"current_container", getter(PyDisplayHandle_current_container), nullptr, nullptr, nullptr},
    {}  /* Sentinel */
};

PyTypeObject PyDisplayHandle = {
    PyVarObject_HEAD_INIT(nullptr, 0)

    "hobbits.DisplayHandle", // const char *tp_name; /* For printing, in format "<module>.<name>" */
    sizeof(DisplayHandlePyObj), // Py_ssize_t tp_basicsize, ; /* For allocation */
    0, //tp_itemsize

    /* Methods to implement standard operations */

    destructor(PyDisplayHandle_dealloc), // destructor tp_dealloc;
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

    "Hobbits Bit Container", // const char *tp_doc; /* Documentation string */

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
    PyDisplayHandle_methods, // struct PyMethodDef *tp_methods;
    PyDisplayHandle_members, // struct PyMemberDef *tp_members;
    PyDisplayHandle_getsets, // struct PyGetSetDef *tp_getset;
    nullptr, // struct _typeobject *tp_base;
    nullptr, // PyObject *tp_dict;
    nullptr, // descrgetfunc tp_descr_get;
    nullptr, // descrsetfunc tp_descr_set;
    0, // Py_ssize_t tp_dictoffset;
    initproc(PyDisplayHandle_init), // initproc tp_init;
    nullptr, // allocfunc tp_alloc;
    PyDisplayHandle_new, // newfunc tp_new;
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
