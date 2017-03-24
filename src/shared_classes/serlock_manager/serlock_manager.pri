INCLUDEPATH += $$PWD
QT += network
#______________ KE_TCP _______________
DEFINES += TC_RECONNECT_TIMEOUT=20000
DEFINES += TC_ALIVE_TIMEOUT=10000

HEADERS += \
    $$PWD/serlock_manager.h
SOURCES += \
    $$PWD/serlock_manager.cpp

win32:LIBS += -lws2_32
