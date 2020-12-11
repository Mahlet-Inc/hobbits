#include "imagebuffer.h"
#include <structmember.h>
#include "math.h"
#define UNUSED(expr) do { (void)(expr); } while (0);

typedef struct {
    PyObject_HEAD
    PyObject* memview;
    unsigned int width;
    unsigned int height;
} ImageBufferPyObj;

static void ImageBuffer_dealloc(ImageBufferPyObj *self)
{
    Py_XDECREF(self->memview);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* ImageBuffer_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    UNUSED(args)
    UNUSED(kwds)
    ImageBufferPyObj *self;
    self = reinterpret_cast<ImageBufferPyObj*>(type->tp_alloc(type, 0));
    if (self != nullptr) {
        self->memview = nullptr;
    }
    return (PyObject*) self;
}

static int ImageBuffer_init(ImageBufferPyObj *self, PyObject *args, PyObject *kwds)
{
    UNUSED(kwds)
    PyObject *memview;
    unsigned int width;
    unsigned int height;
    if (!PyArg_ParseTuple(args, "OII", &memview, &width, &height)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a memoryview, width, and height");
        return -1;
    }

    if (!PyMemoryView_Check(memview)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - hobbits.ImageBuffer must be initialized with a memoryview");
        return -1;
    }

    if (width < 1 || height < 1) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires non-zero width and height");
        return -1;
    }

    Py_buffer * buffer = PyMemoryView_GET_BUFFER(memview);

    if (width * height * 4 != buffer->len) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - memoryview length must be equal to width * height * 4 (4 bytes per pixel)");
        return -1;
    }

    PyBuffer_Release(buffer);

    Py_INCREF(memview);
    self->memview = memview;
    self->width = width;
    self->height = height;
    return 0;
}

static PyObject* ImageBuffer_set_bytes(ImageBufferPyObj *self, PyObject *args)
{
    Py_buffer buffer;
    if (!PyArg_ParseTuple(args, "y*", &buffer)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a bytes-like object");
        return nullptr;
    }

    Py_buffer* memviewBuffer = PyMemoryView_GET_BUFFER(self->memview);

    int toCopy = buffer.len;
    if (toCopy > memviewBuffer->len) {
        toCopy = memviewBuffer->len;
    }

    memcpy(memviewBuffer->buf, buffer.buf, toCopy);

    PyBuffer_Release(&buffer);
    PyBuffer_Release(memviewBuffer);
    Py_RETURN_NONE;
}

static PyObject* ImageBuffer_get_width(ImageBufferPyObj *self, void *Py_UNUSED(closure))
{
    return PyLong_FromUnsignedLong(self->width);
}

static PyObject* ImageBuffer_get_height(ImageBufferPyObj *self, void *Py_UNUSED(closure))
{
    return PyLong_FromUnsignedLong(self->height);
}

static PyMethodDef ImageBuffer_methods[] = {
    { "set_bytes", PyCFunction(ImageBuffer_set_bytes), METH_VARARGS, nullptr },
    {}  /* Sentinel */
};

static PyMemberDef ImageBuffer_members[] = {
    {}  /* Sentinel */
};

static PyGetSetDef ImageBuffer_getsets[] = {
    {"width", getter(ImageBuffer_get_width), nullptr, nullptr, nullptr},
    {"height", getter(ImageBuffer_get_height), nullptr, nullptr, nullptr},
    {}  /* Sentinel */
};

PyTypeObject ImageBuffer = {
    PyVarObject_HEAD_INIT(nullptr, 0)

    "hobbits.ImageBuffer", // const char *tp_name; /* For printing, in format "<module>.<name>" */
    sizeof(ImageBufferPyObj), // Py_ssize_t tp_basicsize, ; /* For allocation */
    0, //tp_itemsize

    /* Methods to implement standard operations */

    destructor(ImageBuffer_dealloc), // destructor tp_dealloc;
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

    "Hobbits Bit Array", // const char *tp_doc; /* Documentation string */

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
    ImageBuffer_methods, // struct PyMethodDef *tp_methods;
    ImageBuffer_members, // struct PyMemberDef *tp_members;
    ImageBuffer_getsets, // struct PyGetSetDef *tp_getset;
    nullptr, // struct _typeobject *tp_base;
    nullptr, // PyObject *tp_dict;
    nullptr, // descrgetfunc tp_descr_get;
    nullptr, // descrsetfunc tp_descr_set;
    0, // Py_ssize_t tp_dictoffset;
    initproc(ImageBuffer_init), // initproc tp_init;
    nullptr, // allocfunc tp_alloc;
    ImageBuffer_new, // newfunc tp_new;
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
