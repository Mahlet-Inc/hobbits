#ifndef PY_BITARRAY_H
#define PY_BITARRAY_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>


extern "C" {

extern PyTypeObject PyBitArray;
extern PyTypeObject PyImmutableBitArray;

}

#endif // PY_BITARRAY_H
