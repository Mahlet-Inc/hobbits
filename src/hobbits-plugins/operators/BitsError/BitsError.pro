#-------------------------------------------------
#
# Project created by QtCreator 2019-06-12T12:32:18
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = BitsError
TEMPLATE = lib
CONFIG += c++11 plugin
CONFIG -= debug_and_release_target

DEFINES += BITSERROR_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        bitserror.cpp

HEADERS += \
        bitserror.h

LIBS += -L$$OUT_PWD/../../../hobbits-core/ -lhobbits-core

INCLUDEPATH += $$PWD/../../../hobbits-core
DEPENDPATH += $$PWD/../../../hobbits-core

unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../../lib:\$$ORIGIN\'"
}


unix {
    target.path = $$(HOME)/.local/share/hobbits/plugins/operators
    INSTALLS += target
}

FORMS += \
    bitserror.ui

DISTFILES +=
