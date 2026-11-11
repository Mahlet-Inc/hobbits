#ifndef HOBBITS_PYTHONAPI_H
#define HOBBITS_PYTHONAPI_H

#ifndef PY_SSIZE_T_CLEAN
#define PY_SSIZE_T_CLEAN
#endif

// Ordinary Windows Python installations normally provide a release ABI only.
// Load CRT headers with the application's debug settings before including
// Python's headers, which otherwise request pythonXY_d.lib automatically.
#if defined(_MSC_VER) && defined(_DEBUG) && defined(HOBBITS_PYTHON_RELEASE_ABI)
#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <io.h>
#pragma push_macro("_DEBUG")
#undef _DEBUG
#include <Python.h>
#pragma pop_macro("_DEBUG")
#else
#include <Python.h>
#endif

#endif // HOBBITS_PYTHONAPI_H
