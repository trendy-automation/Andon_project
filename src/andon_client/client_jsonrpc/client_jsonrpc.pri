include($${ANDNPRJ_LIBPATH}/qjsonrpc/qjsonrpc.pri)
INCLUDEPATH += $${ANDNPRJ_LIBPATH}/qjsonrpc \
               $${ANDNPRJ_LIBPATH}/qjsonrpc/json
#LIBS += -L$${ANDNPRJ_LIBPATH}/qjsonrpc $${QJSONRPC_LIBS}
#LIBS += -L$${QJSONRPCDESTDIR} $${QJSONRPC_LIBS}

QT += core network testlib
CONFIG -= testcase

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/client_rpcutility.h \
    $$PWD/client_rpcservice.h
SOURCES += \
    $$PWD/client_rpcutility.cpp
