QT       += core gui
QT       += widgets

#________________ IM _______________
DEFINES += IM_BUTTONS_UPDATE_INTERVAL=5000

INCLUDEPATH += $$PWD

SOURCES += \  
    $$PWD/interface_manager.cpp

HEADERS  += \
    $$PWD/interface_manager.h

