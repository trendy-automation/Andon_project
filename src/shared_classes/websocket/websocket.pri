QT       += webchannel
QT       += websockets
QT       += network

INCLUDEPATH += $${PWD}

SOURCES += \
    $$PWD/websocketclientwrapper.cpp \
    $$PWD/websockettransport.cpp \

HEADERS += \
    $$PWD/websocketclientwrapper.h \
    $$PWD/websockettransport.h \

win32 {	
    LIBS += $$PWD/libws2_32.a
#    LIBS += C:\Qt\Qt5.6.0\Tools\mingw492_32\i686-w64-mingw32\lib\libws2_32.a
}
