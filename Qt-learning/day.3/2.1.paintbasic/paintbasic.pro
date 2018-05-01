#-------------------------------------------------
#
# Project created by QtCreator 2014-10-24T09:42:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = painterpath
TEMPLATE = app

DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += mainwidget.h paintarea.h
SOURCES += main.cpp mainwidget.cpp paintarea.cpp
RESOURCES += paintbasic.qrc
TRANSLATIONS += paintbasic_zh.ts
