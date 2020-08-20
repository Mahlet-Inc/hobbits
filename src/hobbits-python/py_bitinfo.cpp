#include "py_bitinfo.h"
#include "bitinfo.h"
#include <QSharedPointer>

#include <structmember.h>

#define BITINFO(X) static_cast<BitInfo*>(PyCapsule_GetPointer(X, nullptr))

typedef struct {
    PyObject_HEAD
    PyObject* bitInfoCapsule;
} BitInfoPyObj;

static void BitInfoPy_dealloc(BitInfoPyObj *self)
{
    Py_XDECREF(self->bitInfoCapsule);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* BitInfoPy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    BitInfoPyObj *self;
    self = reinterpret_cast<BitInfoPyObj*>(type->tp_alloc(type, 0));
    if (self != nullptr) {
        self->bitInfoCapsule = nullptr;
    }
    return (PyObject*) self;
}

static int BitInfoPy_init(BitInfoPyObj *self, PyObject *args, PyObject *kwds)
{
    PyObject *bitArrayCapsule;
    if (!PyArg_ParseTuple(args, "O", &bitArrayCapsule)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a bit array capsule");
        return -1;
    }

    Py_INCREF(bitArrayCapsule);
    self->bitInfoCapsule = bitArrayCapsule;
    return 0;
}

static char* addHighlightKwargs[] = {"category", "label", "start", "end", "color", nullptr};
static PyObject* BitInfoPy_add_highlight(BitInfoPyObj *self, PyObject *args, PyObject *kwds)
{
    char *category;
    char *label;
    long long start;
    long long end;
    unsigned int color = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssLL|$I", addHighlightKwargs, &category, &label, &start, &end, &color)) {
        return nullptr;
    }
    if (color == 0) {
        color = 0x44ff8800;
    }
    BitInfo* info = BITINFO(self->bitInfoCapsule);
    info->addHighlight(RangeHighlight::simple(category, label, Range(start, end), color));
    Py_RETURN_NONE;
}

static PyObject* buildRangeHighlight(RangeHighlight highlight) {
    return Py_BuildValue("{sssssLsLsI}",
                         "category", highlight.category().toStdString().c_str(),
                         "label", highlight.label().toStdString().c_str(),
                         "start", highlight.range().start(),
                         "end", highlight.range().end(),
                         "color", highlight.rgbaColor());
}

static char* getHighlightsKwargs[] = {"category", nullptr};
static PyObject* BitInfoPy_get_highlights(BitInfoPyObj *self, PyObject *args, PyObject *kwds)
{
    char *category;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", getHighlightsKwargs, &category)) {
        return nullptr;
    }
    BitInfo* info = BITINFO(self->bitInfoCapsule);
    auto highlights = info->highlights(category);
    PyObject* listResult = PyList_New(highlights.size());
    for (int i = 0; i < highlights.size(); i++) {
        PyList_SetItem(listResult, i, buildRangeHighlight(highlights.at(i)));
    }

    return listResult;
}

static PyMethodDef BitInfoPy_methods[] = {
    { "add_highlight", PyCFunction(BitInfoPy_add_highlight), METH_VARARGS | METH_KEYWORDS, nullptr },
    { "get_highlights", PyCFunction(BitInfoPy_get_highlights), METH_VARARGS | METH_KEYWORDS, nullptr },
    {}  /* Sentinel */
};

static PyMemberDef BitInfoPy_members[] = {
    {}  /* Sentinel */
};

static PyMethodDef ImmutableBitInfoPy_methods[] = {
    { "get_highlights", PyCFunction(BitInfoPy_get_highlights), METH_VARARGS | METH_KEYWORDS, nullptr },
    {}  /* Sentinel */
};

static PyMemberDef ImmutableBitInfoPy_members[] = {
    {}  /* Sentinel */
};

extern PyTypeObject PyBitInfo = {
    PyVarObject_HEAD_INIT(nullptr, 0)

    "hobbits.BitInfo", // const char *tp_name; /* For printing, in format "<module>.<name>" */
    sizeof(BitInfoPyObj), // Py_ssize_t tp_basicsize, ; /* For allocation */
    0, //tp_itemsize

    /* Methods to implement standard operations */

    destructor(BitInfoPy_dealloc), // destructor tp_dealloc;
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

    "Hobbits Bit Info", // const char *tp_doc; /* Documentation string */

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
    BitInfoPy_methods, // struct PyMethodDef *tp_methods;
    BitInfoPy_members, // struct PyMemberDef *tp_members;
    nullptr, // struct PyGetSetDef *tp_getset;
    nullptr, // struct _typeobject *tp_base;
    nullptr, // PyObject *tp_dict;
    nullptr, // descrgetfunc tp_descr_get;
    nullptr, // descrsetfunc tp_descr_set;
    0, // Py_ssize_t tp_dictoffset;
    initproc(BitInfoPy_init), // initproc tp_init;
    nullptr, // allocfunc tp_alloc;
    BitInfoPy_new, // newfunc tp_new;
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

extern PyTypeObject PyImmutableBitInfo = {
    PyVarObject_HEAD_INIT(nullptr, 0)

    "hobbits.ImmutableBitInfo", // const char *tp_name; /* For printing, in format "<module>.<name>" */
    sizeof(BitInfoPyObj), // Py_ssize_t tp_basicsize, ; /* For allocation */
    0, //tp_itemsize

    /* Methods to implement standard operations */

    destructor(BitInfoPy_dealloc), // destructor tp_dealloc;
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

    "Immutable Hobbits Bit Info", // const char *tp_doc; /* Documentation string */

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
    ImmutableBitInfoPy_methods, // struct PyMethodDef *tp_methods;
    ImmutableBitInfoPy_members, // struct PyMemberDef *tp_members;
    nullptr, // struct PyGetSetDef *tp_getset;
    nullptr, // struct _typeobject *tp_base;
    nullptr, // PyObject *tp_dict;
    nullptr, // descrgetfunc tp_descr_get;
    nullptr, // descrsetfunc tp_descr_set;
    0, // Py_ssize_t tp_dictoffset;
    initproc(BitInfoPy_init), // initproc tp_init;
    nullptr, // allocfunc tp_alloc;
    BitInfoPy_new, // newfunc tp_new;
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
