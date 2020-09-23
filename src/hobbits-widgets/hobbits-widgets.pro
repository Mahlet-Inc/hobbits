#-------------------------------------------------
#
# Project created by QtCreator 2020-09-22T10:36:17
#
#-------------------------------------------------

QT       += widgets

TARGET = hobbits-widgets
TEMPLATE = lib

DEFINES += HOBBITSWIDGETS_LIBRARY

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
        batcheditor.cpp \
        actionrenderprogress.cpp \
        bitcontainerlistmodel.cpp \
        bitcontainermanagerui.cpp \
        bitcontainertreemodel.cpp \
        highlightnavigator.cpp \
        displaybase.cpp \
        displaybasetext.cpp \
        displayhandle.cpp \
        displayhelper.cpp \
        plugincallback.cpp \
        pluginstatehelper.cpp \
        previewscrollbar.cpp \
        widgetssettings.cpp

HEADERS += \
        batcheditor.h \
        bitcontainermanagerui.h \
        plugincallback.h \
        pluginstatehelper.h \
        hobbits-widgets_global.h \
        previewscrollbar.h \
        actionrenderprogress.h \
        bitcontainerlistmodel.h \
        bitcontainertreemodel.h \
        displaybase.h \
        displaybasetext.h \
        displayhandle.h \
        displayhelper.h \
        highlightnavigator.h \
        widgetssettings.h

FORMS += \
        highlightnavigator.ui

LIBS += -L$$OUT_PWD/../hobbits-core/ -lhobbits-core
INCLUDEPATH += $$PWD/../hobbits-core
DEPENDPATH += $$PWD/../hobbits-core

headers.files   += $$HEADERS
headers.path    = $$(HOME)/.local/include/hobbits
target.path = $$(HOME)/.local/lib/hobbits
INSTALLS += target headers
