#-------------------------------------------------
#
# Project created by QtCreator 2019-06-04T10:53:29
#
#-------------------------------------------------

QT       -= gui

TARGET = hobbits-core
TEMPLATE = lib

CONFIG += c++11
CONFIG -= debug_and_release_target

DEFINES += HOBBITSCORE_LIBRARY

!contains(DEFINES, HOBBITS_CORE_LIB_VERSION.*) {
    DEFINES += "HOBBITS_CORE_LIB_VERSION=\"\\\"Super Cool Developer Version\\\"\""
}

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    abstractpluginrunner.cpp \
    analyzerresult.cpp \
    analyzerrunner.cpp \
    batchrunner.cpp \
    bitarray.cpp \
    bitcontainer.cpp \
    bitcontainermanager.cpp \
    bitcontainerpreview.cpp \
    bitinfo.cpp \
    displayrenderconfig.cpp \
    exporterrunner.cpp \
    exportresult.cpp \
    frame.cpp \
    hobbitscoreinfo.cpp \
    hobbitspluginmanager.cpp \
    importerrunner.cpp \
    importresult.cpp \
    mathparser.cpp \
    metadatahelper.cpp \
    operatorresult.cpp \
    operatorrunner.cpp \
    parameterdelegate.cpp \
    pluginaction.cpp \
    pluginactionbatch.cpp \
    pluginactionlineage.cpp \
    pluginactionmanager.cpp \
    pluginactionprogress.cpp \
    range.cpp \
    rangehighlight.cpp \
    rangesequence.cpp \
    settingsdata.cpp \
    settingsmanager.cpp

HEADERS += \
    abstractpluginrunner.h \
    analyzerinterface.h \
    analyzerresult.h \
    analyzerrunner.h \
    batchrunner.h \
    bitarray.h \
    bitcontainer.h \
    bitcontainermanager.h \
    bitcontainerpreview.h \
    bitinfo.h \
    displayinterface.h \
    displayrenderconfig.h \
    exporterrunner.h \
    exportresult.h \
    frame.h \
    fwd_abstractparametereditor.h \
    hobbits-core_global.h  \
    hobbitscoreinfo.h \
    hobbitsplugin.h \
    hobbitspluginmanager.h \
    importerrunner.h \
    importexportinterface.h \
    importresult.h \
    metadatahelper.h \
    operatorinterface.h \
    mathparser.h \
    operatorresult.h \
    operatorrunner.h \
    parameterdelegate.h \
    pluginaction.h \
    pluginactionbatch.h \
    pluginactionlineage.h \
    pluginactionmanager.h \
    pluginactionprogress.h \
    pluginactionwatcher.h \
    range.h \
    rangehighlight.h \
    rangesequence.h \
    settingsdata.h \
    settingsmanager.h

DISTFILES +=

RESOURCES +=

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

