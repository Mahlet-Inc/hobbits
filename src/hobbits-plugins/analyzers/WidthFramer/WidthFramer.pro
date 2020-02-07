#-------------------------------------------------
#
# Project created by QtCreator 2019-09-18T16:02:26.935Z
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = WidthFramer
TEMPLATE = lib

DEFINES += WIDTHFRAMER_LIBRARY

CONFIG += c++11 plugin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=         widthframer.cpp \
    peakselector.cpp

HEADERS +=         widthframer.h \
    peakselector.h

FORMS +=        widthframer.ui

DISTFILES +=     

win32 {
    LIBS += -L$$OUT_PWD/../../../../windows -lfftw3-3
    INCLUDEPATH += $$OUT_PWD/../../../../windows
    DEPENDPATH += $$OUT_PWD/../../../../windows

    DEFINES += FFTW_AUTOCORRELATION
}
unix {
    packagesExist(fftw3) {
        LIBS += -lfftw3
        DEFINES += FFTW_AUTOCORRELATION
    }
    else {
        warning("The FFTW3 package could not be found, so WidthFramer will build without autocorrelation")
    }
}

LIBS += -L$$OUT_PWD/../../../hobbits-core/ -lhobbits-core

INCLUDEPATH += $$PWD/../../../hobbits-core
DEPENDPATH += $$PWD/../../../hobbits-core

unix {
    target.path = $$(HOME)/.local/share/hobbits/plugins/analyzers
    INSTALLS += target
}

