#include "py_bitarray.h"
#include "bitarray.h"
#include <QSharedPointer>

#include <python3.7m/structmember.h>

#define BITS(X) static_cast<BitArray*>(PyCapsule_GetPointer(X, nullptr))

typedef struct {
    PyObject_HEAD
    PyObject* bitsCapsule;
} BitArrayPyObj;

static void BitArrayPy_dealloc(BitArrayPyObj *self)
{
    Py_XDECREF(self->bitsCapsule);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* BitArrayPy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    BitArrayPyObj *self;
    self = reinterpret_cast<BitArrayPyObj*>(type->tp_alloc(type, 0));
    if (self != nullptr) {
        self->bitsCapsule = nullptr;
    }
    return (PyObject*) self;
}

static int BitArrayPy_init(BitArrayPyObj *self, PyObject *args, PyObject *kwds)
{
    PyObject *bitArrayCapsule;
    if (!PyArg_ParseTuple(args, "O", &bitArrayCapsule)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a bit array capsule");
        return -1;
    }

    Py_INCREF(bitArrayCapsule);
    self->bitsCapsule = bitArrayCapsule;
    return 0;
}

static PyObject* BitArrayPy_size(BitArrayPyObj *self, PyObject *Py_UNUSED(ignored))
{
    BitArray* bits = BITS(self->bitsCapsule);
    return PyLong_FromLongLong(bits->sizeInBits());
}

static PyObject* BitArrayPy_at(BitArrayPyObj *self, PyObject *args )
{
    long long idx;
    if (!PyArg_ParseTuple(args, "L", &idx)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a bit index");
        return nullptr;
    }

    BitArray* bits = BITS(self->bitsCapsule);
    if (bits->sizeInBits() <= idx || idx < 0) {
        PyErr_SetString(PyExc_IndexError, "provided bit index is not valid");
        return nullptr;
    }

    return PyBool_FromLong(bits->at(idx));
}

static PyObject* BitArrayPy_resize(BitArrayPyObj *self, PyObject *args )
{
    long long size;
    if (!PyArg_ParseTuple(args, "L", &size)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a size in bits");
        return nullptr;
    }

    if (size < 0) {
        PyErr_SetString(PyExc_IndexError, "negative bit array size is not valid");
        return nullptr;
    }

    BitArray* bits = BITS(self->bitsCapsule);
    bits->resize(size);
    Py_RETURN_NONE;
}

static PyObject* BitArrayPy_set(BitArrayPyObj *self, PyObject *args )
{
    long long idx;
    int val;
    if (!PyArg_ParseTuple(args, "Lp", &idx, &val)) {
        PyErr_SetString(PyExc_TypeError, "invalid arguments - requires a bit index and value");
        return nullptr;
    }

    BitArray* bits = BITS(self->bitsCapsule);
    if (bits->sizeInBits() <= idx || idx < 0) {
        PyErr_SetString(PyExc_IndexError, "provided bit index is not valid");
        return nullptr;
    }

    bits->set(idx, val);
    Py_RETURN_NONE;
}

static PyMethodDef BitArrayPy_methods[] = {
    { "size", PyCFunction(BitArrayPy_size), METH_NOARGS, nullptr },
    { "at", PyCFunction(BitArrayPy_at), METH_VARARGS, nullptr },
    { "resize", PyCFunction(BitArrayPy_resize), METH_VARARGS, nullptr },
    { "set", PyCFunction(BitArrayPy_set), METH_VARARGS, nullptr },
    {NULL}  /* Sentinel */
};

static PyMethodDef ImmutableBitArrayPy_methods[] = {
    { "size", PyCFunction(BitArrayPy_size), METH_NOARGS, nullptr },
    { "at", PyCFunction(BitArrayPy_at), METH_VARARGS, nullptr },
    {NULL}  /* Sentinel */
};


static PyMemberDef ImmutableBitArrayPy_members[] = {
    {nullptr}  /* Sentinel */
};

static PyMemberDef BitArrayPy_members[] = {
    {nullptr}  /* Sentinel */
};

extern PyTypeObject PyBitArray = {
    PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "hobbits.BitArray",
    .tp_basicsize = sizeof(BitArrayPyObj),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor) BitArrayPy_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "Hobbits Bit Array",
    .tp_methods = BitArrayPy_methods,
    .tp_members = BitArrayPy_members,
    .tp_init = (initproc) BitArrayPy_init,
    .tp_new = BitArrayPy_new,
};

extern PyTypeObject PyImmutableBitArray = {
    PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "hobbits.ImmutableBitArray",
    .tp_basicsize = sizeof(BitArrayPyObj),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor) BitArrayPy_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "Immutable Hobbits Bit Array",
    .tp_methods = ImmutableBitArrayPy_methods,
    .tp_members = ImmutableBitArrayPy_members,
    .tp_init = (initproc) BitArrayPy_init,
    .tp_new = BitArrayPy_new,
};
