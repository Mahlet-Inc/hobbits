#-------------------------------------------------
#
# Project created by QtCreator 2019-06-04T10:55:32
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = hobbits
TEMPLATE = app

!contains(DEFINES, HOBBITS_GUI_VERSION.*) {
    DEFINES += "HOBBITS_GUI_VERSION=\"\\\"Totally Rad Developer Version\\\"\""
}

DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_RESOURCE_FLAGS += -no-compress

CONFIG += c++11
CONFIG -= debug_and_release_target

SOURCES += \
        batchcreationdialog.cpp \
        containerselectiondialog.cpp \
        hobbitsguiinfo.cpp \
        hobbitsstyle.cpp \
        main.cpp \
        mainwindow.cpp \
        preferencesdialog.cpp

HEADERS += \
        batchcreationdialog.h \
        containerselectiondialog.h \
        hobbitsguiinfo.h \
        hobbitsstyle.h \
        mainwindow.h \
        preferencesdialog.h

FORMS += \
        batchcreationdialog.ui \
        containerselectiondialog.ui \
        mainwindow.ui \
        preferencesdialog.ui

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

    # this is for situations when LD_LIBRARY_PATH won't work for launching hobbits in Qt Creator due to something like setcap
    defined(DEVELOPER_RPATH, var) {
        QMAKE_LFLAGS += "-Wl,-rpath,\'$$OUT_PWD/../hobbits-core:$$OUT_PWD/../hobbits-python:$$OUT_PWD/../hobbits-widgets:$$HOBBITS_PYPATH/lib\'"
    }
    else {
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../lib:\$$ORIGIN/../python/lib:\$$ORIGIN\'"
    }
}

mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'@executable_path/../Frameworks,-rpath,@executable_path/../Frameworks/python/lib\'"
}

# since plugins cannot synchronize fftw planner calls, the GUI must force thread-safe planning if fftw is available
win32-msvc* {
    exists($$OUT_PWD/../../../../windows/libfftw3-3.lib) {
        LIBS += -L$$OUT_PWD/../../../../windows -llibfftw3-3
        INCLUDEPATH += $$OUT_PWD/../../../../windows
        DEPENDPATH += $$OUT_PWD/../../../../windows

        DEFINES += FFTW_AVAILABLE
    }
    else {
        warning("The FFTW3 .lib file could not be found, so the GUI will be unable to force fftw planner thead-safety across plugins")
        warning("Did you turn the .def into a .lib with 'lib.exe /def:libfftw3-3.def' ?")
    }
}
win32-g++ {
    LIBS += -L$$OUT_PWD/../../../../windows -lfftw3-3
    INCLUDEPATH += $$OUT_PWD/../../../../windows
    DEPENDPATH += $$OUT_PWD/../../../../windows

    DEFINES += FFTW_AVAILABLE
}
unix {
    packagesExist(fftw3) {
        mac {
            INCLUDEPATH += /usr/local/include
            LIBS += -L/usr/local/lib
        }
        LIBS += -lfftw3 -lfftw3_threads
        DEFINES += FFTW_AVAILABLE
    }
    else {
        warning("The FFTW3 package could not be found, so the GUI will be unable to force fftw planner thead-safety across plugins")
    }
}

RESOURCES += \
    icons.qrc \
    style.qrc

INSTALLS =


message(qmake hobbits-gui config: $$CONFIG)
message(Building from: $$PWD)
message(Building in: $$OUT_PWD)
message(Target output: $$DESTDIR/$$TARGET)
