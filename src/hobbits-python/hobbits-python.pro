#-------------------------------------------------
#
# Project created by QtCreator 2020-07-30T17:29:04
#
#-------------------------------------------------

requires(defined(HOBBITS_PYPATH, var))
message(HOBBITS_PYPATH: $$HOBBITS_PYPATH)

QT       -= gui

TARGET = hobbits-python
TEMPLATE = lib

CONFIG += c++11
CONFIG -= debug_and_release_target

# For conflict with Python.h "slots" usage
CONFIG += no_keywords

DEFINES += HOBBITSPYTHON_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        hobbitspython.cpp \
        py_actionprogress.cpp \
        py_bitarray.cpp \
        py_bitcontainer.cpp \
        py_bitinfo.cpp \
        py_hobbits.cpp \
        pythonarg.cpp \
        pythoninterpreter.cpp \
        pythonrequest.cpp \
        pythonresult.cpp

HEADERS += \
        hobbitspython.h \
        hobbits-python_global.h  \
        py_actionprogress.h \
        py_bitarray.h \
        py_bitcontainer.h \
        py_bitinfo.h \
        py_hobbits.h \
        pythonarg.h \
        pythoninterpreter.h \
        pythonrequest.h \
        pythonresult.h

LIBS += -L$$OUT_PWD/../hobbits-core/ -lhobbits-core
INCLUDEPATH += $$PWD/../hobbits-core
DEPENDPATH += $$PWD/../hobbits-core

include(python-link.pri)

unix:!mac {
    PY_LIB = $$files($$HOBBITS_PYPATH/lib/*.*.so)
    PY_LIB = $$first(PY_LIB)
    PY_LIB = $$basename(PY_LIB)
}

mac {
    PY_LIB = $$files($$HOBBITS_PYPATH/lib/*.*.dylib)
    PY_LIB = $$first(PY_LIB)
    PY_LIB = $$basename(PY_LIB)
}

win32 {
    PY_LIB = $$files($$HOBBITS_PYPATH/lib/*.lib)
    PY_LIB = $$first(PY_LIB)
    PY_LIB = $$basename(PY_LIB)
}

DEFINES += "PYTHON_LINK_LIB=\"\\\"$$PY_LIB\\\"\""

unix:!mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../python/lib:\$$ORIGIN\'"
}

headers.files   += $$HEADERS
headers.path    = $$(HOME)/.local/include/hobbits/python
target.path = $$(HOME)/.local/lib/hobbits
INSTALLS += target headers

message(qmake config hobbits-python: $$CONFIG)
message(Building from: $$PWD)
message(Building in: $$OUT_PWD)
message(Target output: $$DESTDIR/$$TARGET)
