#ifndef PY_BITINFO_H
#define PY_BITINFO_H

#define PY_SSIZE_T_CLEAN
#include "pythonapi.h"


extern "C" {

extern PyTypeObject PyBitInfo;
extern PyTypeObject PyImmutableBitInfo;

}

#endif // PY_BITINFO_H
