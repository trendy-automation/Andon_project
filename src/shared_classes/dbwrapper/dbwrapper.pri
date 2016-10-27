QT       += sql

INCLUDEPATH+=$$PWD
DEFINES += DATABASE_FILE='\\"ANDON_DB.FDB\\"'

sqlPlugins.path = sqldrivers
DEPLOYMENT += sqlPlugins

HEADERS += \
    $$PWD/dbwrapper.h
SOURCES += \
    $$PWD/dbwrapper.cpp

#    INCLUDEPATH+="C:/Program Files/Firebird/Firebird_2_5/include"
#    LIBS += D:\Distrib\linux\firebird_sql\Firebird-2.5.3.26778-0_Win32\lib\fbclient_ms.lib

