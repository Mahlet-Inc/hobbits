#ifndef PY_BITCONTAINER_H
#define PY_BITCONTAINER_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>


extern "C" {

extern PyTypeObject PyBitContainer;
extern PyTypeObject PyImmutableBitContainer;

}

#endif // PY_BITCONTAINER_H
