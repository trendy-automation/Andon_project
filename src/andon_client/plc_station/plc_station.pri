QT += core network
LIBS        += -lsnap7

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/snap7.h \
    $$PWD/customkey.h \
    $$PWD/plc_station.h

SOURCES += \
    $$PWD/snap7.cpp \
    $$PWD/plc_station.cpp



