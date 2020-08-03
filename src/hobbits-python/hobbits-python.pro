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

!contains(DEFINES, HOBBITS_PYTHON_LIB_VERSION.*) {
    DEFINES += "HOBBITS_PYTHON_LIB_VERSION=\"\\\"Spam, Spam, Eggs, and Spam Version\\\"\""
}

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
LIBS += -lpython3.7m

INCLUDEPATH += $$PWD/../hobbits-core
DEPENDPATH += $$PWD/../hobbits-core

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
