#-------------------------------------------------
#
# Project created by QtCreator 2014-11-16T14:08:54
#
#-------------------------------------------------
QT += widgets testlib

TARGET = guitest
TEMPLATE = app

SOURCES += main.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

# install
target.path = $$PWD/
INSTALLS += target
