#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T22:27:02
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app

# Input
SOURCES += main.cpp \
	server.cpp \
	tcpclientsocket.cpp \
	tcpserver.cpp

HEADERS += server.h \
	tcpclientsocket.h \
	tcpserver.h