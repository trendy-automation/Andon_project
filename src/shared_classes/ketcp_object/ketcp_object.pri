INCLUDEPATH += $$PWD
QT += network
#______________ KE_TCP _______________
DEFINES += KE_WAITFOR_SABILE=700
DEFINES += KE_RECONNECT_TIMEOUT=60000
DEFINES += KE_ALIVE_TIMEOUT=10000
DEFINES += KE_MAX_INPUTCODE_SIZE=32

HEADERS += \
    $$PWD/ketcp_object.h
SOURCES += \
    $$PWD/ketcp_object.cpp

win32:LIBS += -lws2_32
