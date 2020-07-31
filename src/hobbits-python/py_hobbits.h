#ifndef PY_HOBBITS_H
#define PY_HOBBITS_H

#define PY_SSIZE_T_CLEAN
#include <python3.7m/Python.h>

extern "C" {

PyMODINIT_FUNC
PyInit_hobbits(void);

}

#endif // PY_HOBBITS_H
