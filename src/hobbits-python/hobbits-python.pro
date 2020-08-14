#-------------------------------------------------
#
# Project created by QtCreator 2020-07-30T17:29:04
#
#-------------------------------------------------

requires(defined(HOBBITS_PYPATH, var))

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
    LIBS += $$system(pkg-config --with-path=$$HOBBITS_PYPATH/lib/pkgconfig --define-prefix --libs python3-embed)
    PY_INC = $$system(pkg-config --with-path=$$HOBBITS_PYPATH/lib/pkgconfig --define-prefix --cflags python3-embed)
    PY_INC = $$str_member($$PY_INC, 2, -1)
    INCLUDEPATH += $$PY_INC
    DEPENDPATH += $$PY_INC
}

win32 {
    LIBS += -L$$HOBBITS_PYPATH/bin -lpython3
    INCLUDEPATH += $$HOBBITS_PYPATH/include
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
