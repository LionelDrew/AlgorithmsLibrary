###设定编译后的动态库名称
TARGET = $$qtLibraryTarget(talgorithmslib)

###添加QT库依赖库
QT       += core

HEADERS += \
    beziercurve2arcs/beziercurvetoarcs.h \
    beziercurve2arcs/beziermathtools.h \
    beziercurve2arcs/cubicbeziertools.h \
    talgoshapes/talgoarc.h \
    talgoshapes/talgocircle.h \
    talgoshapes/talgofigure.h \
    talgoshapes/talgoline.h \
    talgoshapes/talgoray.h \
    talgoshapes/talgosegment.h \
    talgopathutil.h \
    talgorithmslib_global.h \
    tcommonutil.h \
    tnest/t2dnest.h \
    parse/tgcodeitem.h \
    parse/tgcodeparseclass.h \
    tnest/t2dnestparameter.h \
    tnest/t2dnestsolution.h \
    tnest/t2dnestutil.h \
    tool/tnumberscopelist.h \
    tnest/t2dnestdebuger.h

SOURCES += \
    beziercurve2arcs/beziercurvetoarcs.cpp \
    beziercurve2arcs/beziermathtools.cpp \
    beziercurve2arcs/cubicbeziertools.cpp \
    talgoshapes/talgoarc.cpp \
    talgoshapes/talgocircle.cpp \
    talgoshapes/talgofigure.cpp \
    talgoshapes/talgoline.cpp \
    talgoshapes/talgoray.cpp \
    talgoshapes/talgosegment.cpp \
    talgopathutil.cpp \
    tcommonutil.cpp \
    tnest/t2dnest.cpp \
    parse/gcodeitem.cpp \
    parse/gcodeparseclass.cpp \
    tnest/t2dnestparameter.cpp \
    tnest/t2dnestsolution.cpp \
    tnest/t2dnestutil.cpp \
    tool/tnumberscopelist.cpp \
    tnest/t2dnestdebuger.cpp

RESOURCES += \

#----------------其它编译设定------------------------------------------------
TEMPLATE = lib
DEFINES += TALGORITHMSLIB_LIBRARY

LIBPATH = $$PWD/../..
include(../shared.pri)
###include头文件Path
INCLUDEPATH += .

###导出header
HEADER_PATH = $$system_path($${LIBPATH}/include)
!exists($${HEADER_PATH}) {
    mkpath($${HEADER_PATH})
}
win32{
    system(copy *.h $${HEADER_PATH})
    system(copy talgoshapes\*.h $${HEADER_PATH})
    system(copy tnest\*.h $${HEADER_PATH})
    system(copy parse\*.h $${HEADER_PATH})
    system(copy tool\*.h $${HEADER_PATH})
}
