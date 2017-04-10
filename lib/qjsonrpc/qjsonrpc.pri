QJSONRPC_VERSION = 1.1.0

#include(http-parser/http-parser.pri)
#DEFINES +=QJSONRPC_ALIVE_TIMEOUT=60000

isEmpty(QJSONRPC_LIBRARY_TYPE) {
    QJSONRPC_LIBRARY_TYPE = shared
}

private-headers {
    DEFINES += USE_QT_PRIVATE_HEADERS
    QT += core-private
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/qjsonrpc/release/ -lqjsonrpc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/qjsonrpc/debug/ -lqjsonrpc
else:unix: LIBS += -L$$OUT_PWD/../../lib/qjsonrpc/ -lqjsonrpc

#DEFINES += QJSONRPC_LIB_PATH='\\"{$$OUT_PWD/../../lib/qjsonrpc/release/}qjsonrpc.dll\\"'

INCLUDEPATH += $$PWD #/../../lib/qjsonrpc
DEPENDPATH += $$PWD #/../../lib/qjsonrpc

#QJSONRPCDESTDIR= $$OUT_PWD/../../lib/qjsonrpc/release/
QJSONRPC_INCLUDEPATH = $${PWD}
QJSONRPC_LIBS = -lqjsonrpc
contains(QJSONRPC_LIBRARY_TYPE, staticlib) {
    DEFINES += QJSONRPC_STATIC
} else {
    DEFINES += QJSONRPC_SHARED
    win32:QJSONRPC_LIBS = -lqjsonrpc
}

isEmpty(PREFIX) {
    unix {
        PREFIX = /usr
    } else {
        PREFIX = $$[QT_INSTALL_PREFIX]
    }
}
isEmpty(LIBDIR) {
    LIBDIR = lib
}

win32:CONFIG(release, debug|release): BUILD_TYPE=release
else:win32:CONFIG(debug, debug|release): BUILD_TYPE=debug

copydata.commands = $(COPY_FILE)  $$shell_path($$clean_path($$OUT_PWD/../../lib/qjsonrpc/$$BUILD_TYPE/qjsonrpc.dll))  $$shell_path($$OUT_PWD/$$BUILD_TYPE)
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
