include(../../shared_classes/websocket/websocket.pri)

INCLUDEPATH += $${PWD}

SOURCES += \
    $$PWD/mongoose/mongoose.c

HEADERS += \
    $$PWD/mongoose/mongoose.h \
    $$PWD/serverwebthread.h
