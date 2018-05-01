#-------------------------------------------------
#
# Project created by QtCreator 2015-11-09T15:10:47
#
#-------------------------------------------------

QT       -= gui

TARGET = DLL
TEMPLATE = lib

DEFINES += DLL_LIBRARY

SOURCES += dll.cpp

HEADERS += dll.h\
        dll_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
