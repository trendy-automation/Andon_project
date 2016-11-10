#include(../../shared_classes/keep_alive/keep_alive.pri)

QT += opcua
#CONFIG += qt\
#CONFIG += qt warn_on debug staticlib


INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/opcua_client.h
SOURCES += \
    $$PWD/opcua_client.cpp
