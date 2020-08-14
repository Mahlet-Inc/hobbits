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
    LIBS += -L$$OUT_PWD/../hobbits-python/ -lhobbits-python
    INCLUDEPATH += $$PWD/../hobbits-python
    DEPENDPATH += $$PWD/../hobbits-python

    unix {
        LIBS += $$system(pkg-config --with-path=$$HOBBITS_PYPATH/lib/pkgconfig --define-prefix --libs python3-embed)
        PY_INC = $$system(pkg-config --with-path=$$HOBBITS_PYPATH/lib/pkgconfig --define-prefix --cflags python3-embed)
        PY_INC = $$str_member($$PY_INC, 2, -1)
        INCLUDEPATH += $$PY_INC
        DEPENDPATH += $$PY_INC
    }

    win32 {
        LIBS += -L$$HOBBITS_PYPATH/bin -lpython3
        INCLUDEPATH += $$HOBBITS_PYPATH/include
    }
}

unix:!mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../lib:\$$ORIGIN/../python/lib:\$$ORIGIN\'"
}

mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'@executable_path/../Frameworks:@executable_path/../Frameworks/python/lib\'"
}

RESOURCES += \
    icons.qrc

INSTALLS =


message(qmake hobbits-gui config: $$CONFIG)
message(Building from: $$PWD)
message(Building in: $$OUT_PWD)
message(Target output: $$DESTDIR/$$TARGET)
