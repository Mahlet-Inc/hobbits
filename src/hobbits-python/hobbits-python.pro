#-------------------------------------------------
#
# Project created by QtCreator 2020-07-30T17:29:04
#
#-------------------------------------------------

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
        py_hobbits.h \
        pythonarg.h \
        pythoninterpreter.h \
        pythonrequest.h \
        pythonresult.h

LIBS += -L$$OUT_PWD/../hobbits-core/ -lhobbits-core
INCLUDEPATH += $$PWD/../hobbits-core
DEPENDPATH += $$PWD/../hobbits-core

unix {
    CONFIG *= link_pkgconfig
    PKGCONFIG += python3
}

win32 {
    PYPATH = $$system(python3 -c \"import os; import sys; print(f\'{sys.prefix}{os.path.sep}include{os.path.sep}python{sys.version_info.major}.{sys.version_info.minor}{sys.abiflags}\')\")
    LIBS += -lpython3
    INCLUDEPATH += $$PYPATH
}

unix:!mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

headers.files   += $$HEADERS
headers.path    = $$(HOME)/.local/include/hobbits/python
target.path = $$(HOME)/.local/lib/hobbits
INSTALLS += target headers

message(qmake config hobbits-python: $$CONFIG)
message(Building from: $$PWD)
message(Building in: $$OUT_PWD)
message(Target output: $$DESTDIR/$$TARGET)
