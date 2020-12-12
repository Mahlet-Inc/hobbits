
QT += core widgets

CONFIG += c++11 cmdline
CONFIG -= app_bundle debug_and_release_target

!contains(DEFINES, HOBBITS_RUNNER_VERSION.*) {
    DEFINES += "HOBBITS_RUNNER_VERSION=\"\\\"Extra Good Developer Version\\\"\""
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        hobbitsrunnerinfo.cpp \
        main.cpp

HEADERS += \
    hobbitsrunnerinfo.h

LIBS += -L$$OUT_PWD/../hobbits-core/ -lhobbits-core
LIBS += -L$$OUT_PWD/../hobbits-widgets/ -lhobbits-widgets
INCLUDEPATH += $$PWD/../hobbits-core $$PWD/../hobbits-widgets
DEPENDPATH += $$PWD/../hobbits-core $$PWD/../hobbits-widgets

defined(HOBBITS_PYPATH, var) {
    message(Building hobbits-gui with python support...)
    DEFINES += HAS_EMBEDDED_PYTHON

    LIBS += -L$$OUT_PWD/../hobbits-python/ -lhobbits-python
    INCLUDEPATH += $$PWD/../hobbits-python
    DEPENDPATH += $$PWD/../hobbits-python

    include($$PWD/../hobbits-python/python-link.pri)
}

unix:!mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../lib:\$$ORIGIN/../python/lib:\$$ORIGIN\'"
}

mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'@executable_path/../Frameworks,-rpath,@executable_path/../Frameworks/python/lib\'"
}
