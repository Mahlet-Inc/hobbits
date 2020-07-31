#include "py_hobbits.h"
#include "py_bitarray.h"
#include "py_actionprogress.h"


static struct PyModuleDef HobbitsModuleDef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "hobbits",   /* name of module */
    .m_doc = nullptr, /* module documentation, may be NULL */
    .m_size = 0,       /* size of per-interpreter state of the module,
                        or -1 if the module keeps state in global variables. */
};

PyObject* PyInit_hobbits(void){
    if (PyType_Ready(&PyBitArray) < 0)
        return nullptr;

    if (PyType_Ready(&PyImmutableBitArray) < 0)
        return nullptr;

    if (PyType_Ready(&PyActionProgress) < 0)
        return nullptr;

    PyObject *m = PyModule_Create(&HobbitsModuleDef);
    if (m == nullptr)
        return nullptr;

    Py_INCREF(&PyBitArray);
    Py_INCREF(&PyImmutableBitArray);
    Py_INCREF(&PyActionProgress);
    if (PyModule_AddObject(m, "BitArray", (PyObject*)&PyBitArray) < 0
            || PyModule_AddObject(m, "ImmutableBitArray", (PyObject*)&PyImmutableBitArray) < 0
            || PyModule_AddObject(m, "ActionProgress", (PyObject*)&PyActionProgress) < 0) {
        Py_DECREF(&PyBitArray);
        Py_DECREF(&PyImmutableBitArray);
        Py_DECREF(&PyActionProgress);
        Py_DECREF(m);
        return nullptr;
    }

    return m;
}
