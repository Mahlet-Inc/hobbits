#-------------------------------------------------
#
# Project created by QtCreator 2019-06-05T17:13:08
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = TakeSkip
TEMPLATE = lib
CONFIG += c++11 plugin
CONFIG -= debug_and_release_target

DEFINES += TAKESKIP_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        bitop.cpp \
        invertop.cpp \
        oneop.cpp \
        reverseop.cpp \
        skipop.cpp \
        takeop.cpp \
        takeskip.cpp \
        takeskipeditor.cpp \
        zeroop.cpp

HEADERS += \
        bitop.h \
        invertop.h \
        oneop.h \
        reverseop.h \
        skipop.h \
        takeop.h \
        takeskip.h \
        takeskipeditor.h \
        zeroop.h

FORMS += \
    takeskipeditor.ui

DISTFILES +=

LIBS += -L$$OUT_PWD/../../../hobbits-core/ -lhobbits-core
LIBS += -L$$OUT_PWD/../../../hobbits-widgets/ -lhobbits-widgets
INCLUDEPATH += $$PWD/../../../hobbits-core $$PWD/../../../hobbits-widgets
DEPENDPATH += $$PWD/../../../hobbits-core $$PWD/../../../hobbits-widgets

unix:!mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../../lib:\$$ORIGIN\'"
}

mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'@executable_path/../Frameworks\'"
}

unix {
    target.path = $$(HOME)/.local/share/hobbits/plugins/operators
    INSTALLS += target
}
