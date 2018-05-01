#-------------------------------------------------
#
# Project created by QtCreator 2015-11-09T15:48:38
#
#-------------------------------------------------

QT     += core gui widgets

TARGET = DLL
TEMPLATE = lib

DEFINES += DLL_LIBRARY

SOURCES += dll.cpp \
    form.cpp

HEADERS += dll.h\
        dll_global.h \
    form.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    form.ui
