# never use foreach or Q_FOREACH
DEFINES += QT_NO_FOREACH

win32-msvc* {
    # support windows xp
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
    QMAKE_CXXFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /DEBUG
}

INCLUDEPATH += $${LIBPATH}/include

# support c++11
CONFIG += c++11

# debug suffix
CONFIG += debug_and_release
CONFIG(debug, debug | release) {
    DESTDIR = $${LIBPATH}/bin_dbg
    win32 {
        DEBUG_SUFFIX = d
    }
    macx {
        DEBUG_SUFFIX = _debug
    }
} else {
    DESTDIR = $${LIBPATH}/bin
    DEBUG_SUFFIX =
}

LIBS += -L$$DESTDIR
for (lib, LIB_LIST) {
    LIBS += -l$${lib}$${DEBUG_SUFFIX}
}
