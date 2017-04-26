QT       += sql

INCLUDEPATH+=$$PWD
#INCLUDEPATH+= $$PWD/../../../lib/sqldrivers/IbppDriver/ibpp2531/core

DEFINES += DATABASE_FILE='\\"ANDON_DB.FDB\\"'

sqlPlugins.path = sqldrivers
DEPLOYMENT += sqlPlugins

HEADERS += \
#    $$PWD/../../../lib/sqldrivers/IbppDriver/ibpp2531/core/ibpp.h \
    $$PWD/dbwrapper.h
SOURCES += \
    $$PWD/dbwrapper.cpp

#    INCLUDEPATH+="C:/Program Files/Firebird/Firebird_2_5/include"
#    LIBS += D:\Distrib\linux\firebird_sql\Firebird-2.5.3.26778-0_Win32\lib\fbclient_ms.lib

CONFIG(release, debug|release){BUILD_TYPE=release}
CONFIG(debug, debug|release){BUILD_TYPE=debug}

copydata.commands = $(COPY_FILE) \"$$shell_path($$clean_path($$OUT_PWD/../../lib/ibase/$$BUILD_TYPE/qsqlibase.dll))\" \"$$shell_path($$OUT_PWD/$$BUILD_TYPE/sqldrivers)\"
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
