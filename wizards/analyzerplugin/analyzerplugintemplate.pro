#-------------------------------------------------
#
# Project created by QtCreator %{JS: (new Date().toISOString())}
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = %{ClassName}
TEMPLATE = lib

DEFINES += %{JS: '%{ClassName}'.toUpperCase()}_LIBRARY

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

SOURCES += \
        %{JS: '%{CppFileName}'.toLowerCase()}

HEADERS += \
        %{JS: '%{HeaderFileName}'.toLowerCase()}

FORMS += \
       %{JS: '%{UiFileName}'.toLowerCase()}

DISTFILES += \
     %{JS: '%{JSONFileName}'.toLowerCase()}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../hobbits-core/release/ -lhobbits-core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../hobbits-core/debug/ -lhobbits-core
else:unix: LIBS += -L$$OUT_PWD/../../../hobbits-core/ -lhobbits-core

INCLUDEPATH += $$PWD/../../../hobbits-core
DEPENDPATH += $$PWD/../../../hobbits-core

unix {
    target.path = target.path = $$(HOME)/.local/share/hobbits/plugins/analyzers
    INSTALLS += target
}
