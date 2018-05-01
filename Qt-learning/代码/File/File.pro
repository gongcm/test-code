#-------------------------------------------------
#
# Project created by QtCreator 2016-11-05T21:26:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = File
TEMPLATE = app


SOURCES += main.cpp \
    widget.cpp \
    button.cpp

HEADERS  += widget.h \
    button.h

FORMS    += widget.ui

QMAKE_MAC_SDK = macosx10.12

