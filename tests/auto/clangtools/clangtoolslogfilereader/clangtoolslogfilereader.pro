include(../clangtoolstest.pri)

TARGET = tst_clangtoolslogfilereader

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    tst_clangtoolslogfilereader.cpp \
    $$PLUGINDIR/clangtoolsdiagnostic.cpp \
    $$PLUGINDIR/clangtoolslogfilereader.cpp

HEADERS += \
    $$PLUGINDIR/clangtoolsdiagnostic.h \
    $$PLUGINDIR/clangtoolslogfilereader.h