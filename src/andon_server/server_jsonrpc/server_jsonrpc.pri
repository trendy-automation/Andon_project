include($${ANDNPRJ_LIBPATH}/qjsonrpc/qjsonrpc.pri)
INCLUDEPATH += $${ANDNPRJ_LIBPATH}/qjsonrpc \
               $${ANDNPRJ_LIBPATH}/qjsonrpc/json
LIBS += -L$${ANDNPRJ_LIBPATH}/qjsonrpc $${QJSONRPC_LIBS}



INCLUDEPATH+=$$PWD

HEADERS += \
    $$PWD/server_rpcservice.h \
    $$PWD/server_rpcutility.h
SOURCES += \
    $$PWD/server_rpcservice.cpp \
    $$PWD/server_rpcutility.cpp

