#-------------------------------------------------
#
# Project created by QtCreator 2019-06-04T10:53:29
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = hobbits-core
TEMPLATE = lib

CONFIG += c++11
CONFIG -= debug_and_release_target

DEFINES += HOBBITSCORE_LIBRARY

!contains(DEFINES, HOBBITS_CORE_LIB_VERSION.*) {
    DEFINES += "HOBBITS_CORE_LIB_VERSION=\"\\\"Super Cool Developer Version\\\"\""
}

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
    actionprogress.cpp \
    actionrenderprogress.cpp \
    analyzerresult.cpp \
    analyzerrunner.cpp \
    batchrunner.cpp \
        bitarray.cpp \
        bitcontainer.cpp \
    bitcontainerlistmodel.cpp \
    bitcontainermanager.cpp \
    bitcontainerpreview.cpp \
    bitcontainertreemodel.cpp \
    bitinfo.cpp \
        displaybase.cpp \
    displaybasetext.cpp \
    displayhandle.cpp \
    displayhelper.cpp \
    exportresult.cpp \
        frame.cpp \
    highlightnavigator.cpp \
    hobbitscoreinfo.cpp \
    hobbitspluginmanager.cpp \
    importresult.cpp \
        mathparser.cpp \
    operatorresult.cpp \
    operatorrunner.cpp \
    parseresult.cpp \
    pluginaction.cpp \
    pluginactionbatch.cpp \
    pluginactionlineage.cpp \
    pluginactionmanager.cpp \
    plugincallback.cpp \
    pluginstatehelper.cpp \
    previewscrollbar.cpp \
    range.cpp \
    rangehighlight.cpp \
    rangesequence.cpp \
    settingsdata.cpp \
    settingsmanager.cpp

HEADERS += \
    actionprogress.h \
    actionrenderprogress.h \
    actionwatcher.h \
    analyzerinterface.h \
    analyzerresult.h \
    analyzerrunner.h \
    batchrunner.h \
        bitarray.h \
        bitcontainer.h \
    bitcontainerlistmodel.h \
    bitcontainermanager.h \
    bitcontainerpreview.h \
    bitcontainertreemodel.h \
    bitinfo.h \
        displaybase.h \
    displaybasetext.h \
    displayhandle.h \
    displayhelper.h \
    displayinterface.h \
    exportresult.h \
        frame.h \
    highlightnavigator.h \
        hobbits-core_global.h  \
    hobbitscoreinfo.h \
    hobbitspluginmanager.h \
    importexportinterface.h \
    importresult.h \
        operatorinterface.h \
        mathparser.h \
    operatorresult.h \
    operatorrunner.h \
    parseresult.h \
    pluginaction.h \
    pluginactionbatch.h \
    pluginactionlineage.h \
    pluginactionmanager.h \
    plugincallback.h \
    pluginstatehelper.h \
    previewscrollbar.h \
    range.h \
    rangehighlight.h \
    rangesequence.h \
    settingsdata.h \
    settingsmanager.h

DISTFILES +=

headers.files   += $$HEADERS
headers.path    = $$(HOME)/.local/include/hobbits
target.path = $$(HOME)/.local/lib/hobbits
INSTALLS += target headers

unix:!mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

message(qmake config hobbits-core: $$CONFIG)
message(Building from: $$PWD)
message(Building in: $$OUT_PWD)
message(Target output: $$DESTDIR/$$TARGET)

FORMS += \
    highlightnavigator.ui

RESOURCES += \
    hobbitscoreicons.qrc
