#-------------------------------------------------
#
# Project created by QtCreator 2019-06-04T10:55:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hobbits
TEMPLATE = app

!contains(DEFINES, HOBBITS_GUI_VERSION.*) {
    DEFINES += "HOBBITS_GUI_VERSION=\"\\\"Totally Rad Developer Version\\\"\""
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_RESOURCE_FLAGS += -no-compress

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
CONFIG -= debug_and_release_target

SOURCES += \
        batchcreationdialog.cpp \
        containerselectiondialog.cpp \
        hobbitsguiinfo.cpp \
        main.cpp \
        mainwindow.cpp \
        preferencesdialog.cpp

HEADERS += \
        batchcreationdialog.h \
        containerselectiondialog.h \
        hobbitsguiinfo.h \
        mainwindow.h \
        preferencesdialog.h

FORMS += \
        batchcreationdialog.ui \
        containerselectiondialog.ui \
        mainwindow.ui \
        preferencesdialog.ui

LIBS += -L$$OUT_PWD/../hobbits-core/ -lhobbits-core
INCLUDEPATH += $$PWD/../hobbits-core
DEPENDPATH += $$PWD/../hobbits-core

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

    # this is for situations when LD_LIBRARY_PATH won't work for launching hobbits in Qt Creator due to something like setcap
    defined(DEVELOPER_RPATH, var) {
        QMAKE_LFLAGS += "-Wl,-rpath,\'$$OUT_PWD/../hobbits-core:$$OUT_PWD/../hobbits-python:$$HOBBITS_PYPATH/lib\'"
    }
    else {
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../lib:\$$ORIGIN/../python/lib:\$$ORIGIN\'"
    }
}

mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'@executable_path/../Frameworks,-rpath,@executable_path/../Frameworks/python/lib\'"
}

RESOURCES += \
    icons.qrc

INSTALLS =


message(qmake hobbits-gui config: $$CONFIG)
message(Building from: $$PWD)
message(Building in: $$OUT_PWD)
message(Target output: $$DESTDIR/$$TARGET)
