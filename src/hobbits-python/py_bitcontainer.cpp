#include "py_bitcontainer.h"
#include "bitcontainer.h"
#include <QSharedPointer>

#include <structmember.h>

#define UNUSED(expr) do { (void)(expr); } while (0);
#define BITCON(X) static_cast<BitContainer*>(PyCapsule_GetPointer(X, nullptr))

typedef struct {
    PyObject_HEAD
    PyObject* bitContainerCapsule;
} BitContainerPyObj;

static void BitContainerPy_dealloc(BitContainerPyObj *self)
{
    Py_XDECREF(self->bitContainerCapsule);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* BitContainerPy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    UNUSED(args)
    UNUSED(kwds)
    BitContainerPyObj *self;
    self = reinterpret_cast<BitContainerPyObj*>(type->tp_alloc(type, 0));
    if (self != nullptr) {
        self->bitContainerCapsule = nullptr;
    }
    return (PyObject*) self;
}

static int BitContainerPy_init(BitContainerPyObj *self, PyObject *args, PyObject *kwds)
{
    UNUSED(kwds)
    PyObject *bitContainerCapsule;
    if (!PyArg_ParseTuple(args, "O", &bitContainerCapsule)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a bit array capsule");
        return -1;
    }

    Py_INCREF(bitContainerCapsule);
    self->bitContainerCapsule = bitContainerCapsule;
    return 0;
}

static PyObject* BitContainerPy_get_bits(BitContainerPyObj *self, void *Py_UNUSED(closure))
{
    BitContainer* container = BITCON(self->bitContainerCapsule);

    PyObject* hobbitsModuleName = PyUnicode_FromString("hobbits");
    PyObject* hobbitsModule = PyImport_GetModule(hobbitsModuleName);
    Py_XDECREF(hobbitsModuleName);
    PyObject* type = PyObject_GetAttrString(hobbitsModule, "ImmutableBitArray");
    PyObject* capsule = PyCapsule_New(const_cast<BitArray*>(container->bits().data()), nullptr, nullptr);
    PyObject* bits = PyObject_CallFunction(type, "O", capsule);
    Py_DECREF(type);
    Py_DECREF(capsule);

    return bits;
}

static PyObject* BitContainerPy_get_info(BitContainerPyObj *self, void *Py_UNUSED(closure))
{
    BitContainer* container = BITCON(self->bitContainerCapsule);

    PyObject* hobbitsModuleName = PyUnicode_FromString("hobbits");
    PyObject* hobbitsModule = PyImport_GetModule(hobbitsModuleName);
    Py_XDECREF(hobbitsModuleName);
    PyObject* type = PyObject_GetAttrString(hobbitsModule, "BitInfo");
    PyObject* capsule = PyCapsule_New(container->info().data(), nullptr, nullptr);
    PyObject* info = PyObject_CallFunction(type, "O", capsule);
    Py_DECREF(type);
    Py_DECREF(capsule);

    return info;
}

static PyObject* ImmutableBitContainerPy_get_info(BitContainerPyObj *self, void *Py_UNUSED(closure))
{
    BitContainer* container = BITCON(self->bitContainerCapsule);

    PyObject* hobbitsModuleName = PyUnicode_FromString("hobbits");
    PyObject* hobbitsModule = PyImport_GetModule(hobbitsModuleName);
    Py_XDECREF(hobbitsModuleName);
    PyObject* type = PyObject_GetAttrString(hobbitsModule, "ImmutableBitInfo");
    PyObject* capsule = PyCapsule_New(container->info().data(), nullptr, nullptr);
    PyObject* info = PyObject_CallFunction(type, "O", capsule);
    Py_DECREF(type);
    Py_DECREF(capsule);

    return info;
}

static PyMethodDef BitContainerPy_methods[] = {
    {}  /* Sentinel */
};

static PyMemberDef BitContainerPy_members[] = {
    {}  /* Sentinel */
};

static PyGetSetDef BitContainerPy_getsets[] = {
    {"bits", getter(BitContainerPy_get_bits), nullptr, nullptr, nullptr},
    {"info", getter(BitContainerPy_get_info), nullptr, nullptr, nullptr},
    {}  /* Sentinel */
};

static PyMethodDef ImmutableBitContainerPy_methods[] = {
    {}  /* Sentinel */
};

static PyMemberDef ImmutableBitContainerPy_members[] = {
    {}  /* Sentinel */
};

static PyGetSetDef ImmutableBitContainerPy_getsets[] = {
    {"bits", getter(BitContainerPy_get_bits), nullptr, nullptr, nullptr},
    {"info", getter(ImmutableBitContainerPy_get_info), nullptr, nullptr, nullptr},
    {}  /* Sentinel */
};

PyTypeObject PyBitContainer = {
    PyVarObject_HEAD_INIT(nullptr, 0)

    "hobbits.BitContainer", // const char *tp_name; /* For printing, in format "<module>.<name>" */
    sizeof(BitContainerPyObj), // Py_ssize_t tp_basicsize, ; /* For allocation */
    0, //tp_itemsize

    /* Methods to implement standard operations */

    destructor(BitContainerPy_dealloc), // destructor tp_dealloc;
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
    BitContainerPy_methods, // struct PyMethodDef *tp_methods;
    BitContainerPy_members, // struct PyMemberDef *tp_members;
    BitContainerPy_getsets, // struct PyGetSetDef *tp_getset;
    nullptr, // struct _typeobject *tp_base;
    nullptr, // PyObject *tp_dict;
    nullptr, // descrgetfunc tp_descr_get;
    nullptr, // descrsetfunc tp_descr_set;
    0, // Py_ssize_t tp_dictoffset;
    initproc(BitContainerPy_init), // initproc tp_init;
    nullptr, // allocfunc tp_alloc;
    BitContainerPy_new, // newfunc tp_new;
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

PyTypeObject PyImmutableBitContainer = {
    PyVarObject_HEAD_INIT(nullptr, 0)

    "hobbits.ImmutableBitContainer", // const char *tp_name; /* For printing, in format "<module>.<name>" */
    sizeof(BitContainerPyObj), // Py_ssize_t tp_basicsize, ; /* For allocation */
    0, //tp_itemsize

    /* Methods to implement standard operations */

    destructor(BitContainerPy_dealloc), // destructor tp_dealloc;
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

    "Immutable Hobbits Bit Container", // const char *tp_doc; /* Documentation string */

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
    ImmutableBitContainerPy_methods, // struct PyMethodDef *tp_methods;
    ImmutableBitContainerPy_members, // struct PyMemberDef *tp_members;
    ImmutableBitContainerPy_getsets, // struct PyGetSetDef *tp_getset;
    nullptr, // struct _typeobject *tp_base;
    nullptr, // PyObject *tp_dict;
    nullptr, // descrgetfunc tp_descr_get;
    nullptr, // descrsetfunc tp_descr_set;
    0, // Py_ssize_t tp_dictoffset;
    initproc(BitContainerPy_init), // initproc tp_init;
    nullptr, // allocfunc tp_alloc;
    BitContainerPy_new, // newfunc tp_new;
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
