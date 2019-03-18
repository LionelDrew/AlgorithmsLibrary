QT       += widgets

TARGET = figure
TEMPLATE = app

SOURCES += main.cpp

LIB_LIST = talgorithmslib

LIBPATH = $$PWD/../../..
include(../../shared.pri)

HEADERS += \

