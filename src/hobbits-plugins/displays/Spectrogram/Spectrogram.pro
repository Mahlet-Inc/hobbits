#-------------------------------------------------
#
# Project created by QtCreator 2020-08-21T15:05:39.404Z
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = Spectrogram
TEMPLATE = lib

DEFINES += SPECTROGRAM_LIBRARY

CONFIG += c++11 plugin
CONFIG -= debug_and_release_target

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=         spectrogram.cpp         spectrogramwidget.cpp         spectrogramcontrols.cpp \
    spectrogramrenderer.cpp

HEADERS +=         spectrogram.h         spectrogramwidget.h         spectrogramcontrols.h \
    spectrogramrenderer.h \
    viridis.h

FORMS +=         spectrogramcontrols.ui

DISTFILES +=

RESOURCES += 

LIBS += -L$$OUT_PWD/../../../hobbits-core/ -lhobbits-core
LIBS += -L$$OUT_PWD/../../../hobbits-widgets/ -lhobbits-widgets
INCLUDEPATH += $$PWD/../../../hobbits-core $$PWD/../../../hobbits-widgets
DEPENDPATH += $$PWD/../../../hobbits-core $$PWD/../../../hobbits-widgets

win32-msvc* {
    requires(exists($$OUT_PWD/../../../../windows/fftw3.h))
    requires(exists($$OUT_PWD/../../../../windows/libfftw3-3.lib))
    LIBS += -L$$OUT_PWD/../../../../windows -llibfftw3-3
    INCLUDEPATH += $$OUT_PWD/../../../../windows
    DEPENDPATH += $$OUT_PWD/../../../../windows
}
win32-g++ {
    requires(exists($$OUT_PWD/../../../../windows/fftw3.h))
    requires(exists($$OUT_PWD/../../../../windows/libfftw3-3.dll))
    LIBS += -L$$OUT_PWD/../../../../windows -lfftw3-3
    INCLUDEPATH += $$OUT_PWD/../../../../windows
    DEPENDPATH += $$OUT_PWD/../../../../windows
}
unix {
    requires(packagesExist(fftw3))
    mac {
        INCLUDEPATH += /usr/local/include
        LIBS += -L/usr/local/lib
    }
    LIBS += -lfftw3
}

unix:!mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,'$$ORIGIN/../../lib:$$ORIGIN'"
}

mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,'@executable_path/../Frameworks'"
}

unix {
    target.path = $$(HOME)/.local/share/hobbits/plugins/displays
    INSTALLS += target
}
