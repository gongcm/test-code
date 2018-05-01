#-------------------------------------------------
#
# Project created by QtCreator 2014-10-24T09:42:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = itemtransform
DEPENDPATH += . release
INCLUDEPATH += .

# Input
HEADERS += items.h mainwidget.h
SOURCES += items.cpp main.cpp mainwidget.cpp
RESOURCES += \
    itemtransform.qrc
