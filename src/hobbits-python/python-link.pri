unix:!mac {
    PY_LIB = $$files($$HOBBITS_PYPATH/lib/*.*.so)
    PY_LIB = $$first(PY_LIB)
    PY_LIB = $$basename(PY_LIB)
    PY_LIB = -l$$str_member($$PY_LIB, 3, $$num_add($$str_size($$PY_LIB), -4))
    LIBS += -L$$HOBBITS_PYPATH/lib $$PY_LIB
    PY_INC = $$files($$HOBBITS_PYPATH/include/python3.*)
    PY_INC = $$first(PY_INC)
    INCLUDEPATH += $$PY_INC
    DEPENDPATH += $$PY_INC
}

mac {
    PY_LIB = $$files($$HOBBITS_PYPATH/lib/*.*.dylib)
    PY_LIB = $$first(PY_LIB)
    PY_LIB = $$basename(PY_LIB)
    PY_LIB = -l$$str_member($$PY_LIB, 3, $$num_add($$str_size($$PY_LIB), -7))
    LIBS += -L$$HOBBITS_PYPATH/lib $$PY_LIB
    PY_INC = $$files($$HOBBITS_PYPATH/include/python3.*)
    PY_INC = $$first(PY_INC)
    INCLUDEPATH += $$PY_INC
    DEPENDPATH += $$PY_INC
}

win32 {
    LIBS += -L$$HOBBITS_PYPATH/bin -lpython38
    INCLUDEPATH += $$HOBBITS_PYPATH/include
}
