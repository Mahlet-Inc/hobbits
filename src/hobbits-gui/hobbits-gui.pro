#-------------------------------------------------
#
# Project created by QtCreator 2019-06-04T10:55:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hobbits
TEMPLATE = app

DEFINES += "HOBBITS_GUI_VERSION=\"\\\"Totally Rad Developer Version\\\"\""

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        hobbitsguiinfo.cpp \
        main.cpp \
        mainwindow.cpp \
        preferencesdialog.cpp

HEADERS += \
        hobbitsguiinfo.h \
        mainwindow.h \
        preferencesdialog.h

FORMS += \
        mainwindow.ui \
        preferencesdialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../hobbits-core/ -lhobbits-core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../hobbits-core/ -lhobbits-core
else:unix: LIBS += -L$$OUT_PWD/../hobbits-core/ -lhobbits-core

INCLUDEPATH += $$PWD/../hobbits-core
DEPENDPATH += $$PWD/../hobbits-core

RESOURCES += \
    icons.qrc

INSTALLS =
