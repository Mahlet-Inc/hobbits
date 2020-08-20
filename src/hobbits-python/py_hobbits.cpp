#include "py_hobbits.h"
#include "py_bitarray.h"
#include "py_actionprogress.h"
#include "py_bitinfo.h"
#include "py_bitcontainer.h"


static struct PyModuleDef HobbitsModuleDef = {
    PyModuleDef_HEAD_INIT, // PyModuleDef_Base m_base;
    "hobbits", // const char* m_name;
    nullptr, // const char* m_doc;
    0, // Py_ssize_t m_size;
    nullptr, // PyMethodDef *m_methods;
    nullptr, // struct PyModuleDef_Slot* m_slots;
    nullptr, // traverseproc m_traverse;
    nullptr, // inquiry m_clear;
    nullptr, // freefunc m_free;
};

PyObject* PyInit_hobbits(void){
    if (PyType_Ready(&PyBitArray) < 0)
        return nullptr;

    if (PyType_Ready(&PyImmutableBitArray) < 0)
        return nullptr;

    if (PyType_Ready(&PyActionProgress) < 0)
        return nullptr;

    if (PyType_Ready(&PyBitContainer) < 0)
        return nullptr;

    if (PyType_Ready(&PyImmutableBitContainer) < 0)
        return nullptr;

    if (PyType_Ready(&PyBitInfo) < 0)
        return nullptr;

    if (PyType_Ready(&PyImmutableBitInfo) < 0)
        return nullptr;

    PyObject *m = PyModule_Create(&HobbitsModuleDef);
    if (m == nullptr)
        return nullptr;

    Py_INCREF(&PyBitArray);
    Py_INCREF(&PyImmutableBitArray);
    Py_INCREF(&PyActionProgress);
    Py_INCREF(&PyBitInfo);
    Py_INCREF(&PyImmutableBitInfo);
    Py_INCREF(&PyBitContainer);
    Py_INCREF(&PyImmutableBitContainer);
    if (PyModule_AddObject(m, "BitArray", (PyObject*)&PyBitArray) < 0
            || PyModule_AddObject(m, "ImmutableBitArray", (PyObject*)&PyImmutableBitArray) < 0
            || PyModule_AddObject(m, "ActionProgress", (PyObject*)&PyActionProgress) < 0
            || PyModule_AddObject(m, "BitInfo", (PyObject*)&PyBitInfo) < 0
            || PyModule_AddObject(m, "ImmutableBitInfo", (PyObject*)&PyImmutableBitInfo) < 0
            || PyModule_AddObject(m, "BitContainer", (PyObject*)&PyBitContainer) < 0
            || PyModule_AddObject(m, "ImmutableBitContainer", (PyObject*)&PyImmutableBitContainer) < 0) {
        Py_DECREF(&PyBitArray);
        Py_DECREF(&PyImmutableBitArray);
        Py_DECREF(&PyActionProgress);
        Py_DECREF(&PyBitInfo);
        Py_DECREF(&PyImmutableBitInfo);
        Py_DECREF(&PyBitContainer);
        Py_DECREF(&PyImmutableBitContainer);
        Py_DECREF(m);
        return nullptr;
    }

    return m;
}
