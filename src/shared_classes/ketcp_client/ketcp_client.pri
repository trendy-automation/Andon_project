INCLUDEPATH += $$PWD
QT += core network
#______________ KE_TCP _______________
DEFINES += KE_WAITFOR_SABILE=200
DEFINES += KE_RECONNECT_TIMEOUT=60000
DEFINES += KE_ALIVE_TIMEOUT=10000

HEADERS += \
    $$PWD/ketcp_client.h
SOURCES += \
    $$PWD/ketcp_client.cpp

win32:LIBS += -lws2_32

