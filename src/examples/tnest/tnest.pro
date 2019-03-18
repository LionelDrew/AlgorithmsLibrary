#-------------------------------------------------
#
# Project created by QtCreator 2019-02-25T16:54:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tnest
TEMPLATE = app


LIB_LIST = talgorithmslib

LIBPATH = $$PWD/../../..
include(../../shared.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    tgraphicsview.cpp

HEADERS  += mainwindow.h \
    tgraphicsview.h

FORMS    +=
