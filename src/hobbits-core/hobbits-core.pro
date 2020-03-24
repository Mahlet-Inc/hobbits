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
    analyzeractor.cpp \
    analyzerresult.cpp \
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
        frame.cpp \
    hobbitscoreinfo.cpp \
        mathparser.cpp \
    operatoractor.cpp \
    operatorresult.cpp \
    parseresult.cpp \
    pluginaction.cpp \
    pluginactionlineage.cpp \
    pluginactionmanager.cpp \
    plugincallback.cpp \
    pluginhelper.cpp \
    pluginmanager.cpp \
    range.cpp \
    rangehighlight.cpp \
    settingsdata.cpp \
    settingsmanager.cpp \
    templatefilehandler.cpp

HEADERS += \
    actionprogress.h \
    actionwatcher.h \
    analyzeractor.h \
    analyzerinterface.h \
    analyzerresult.h \
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
        frame.h \
        hobbits-core_global.h  \
    hobbitscoreinfo.h \
    importexportinterface.h \
    operatoractor.h \
        operatorinterface.h \
        mathparser.h \
    operatorresult.h \
    parseresult.h \
    pluginaction.h \
    pluginactionlineage.h \
    pluginactionmanager.h \
    plugincallback.h \
    pluginhelper.h \
    pluginmanager.h \
    range.h \
    rangehighlight.h \
    settingsdata.h \
    settingsmanager.h \
    templatefilehandler.h

DISTFILES +=

INSTALLS =

unix:!mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

message(qmake config hobbits-core: $$CONFIG)
message(Building from: $$PWD)
message(Building in: $$OUT_PWD)
message(Target output: $$DESTDIR/$$TARGET)
