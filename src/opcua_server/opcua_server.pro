include(../../andon_project.pri)
include(../shared_classes/ketcp_object/ketcp_object.pri)
#TODO write RPC. DB from server
include(../shared_classes/dbwrapper/dbwrapper.pri)
include(../shared_classes/message_handler/message_handler.pri)
include(../shared_classes/single_apprun/single_apprun.pri)
include(websocket_class/websocket_class.pri)
include(nodes_manager/nodes_manager.pri)

#_______________ message_handler _______________
DEFINES += DEBUG_NAME='\\"OPC\\"'
DEFINES += APP_NAME='\\"OPC\\"'
DEFINES += APP_VER='\\"0.2\\"'


TEMPLATE = app
TARGET = opcua_server

INCLUDEPATH += $$PWD

CONFIG += c++14
CONFIG += exceptions 
CONFIG += console
CONFIG += link_pkgconfig

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libopcuaprotocol libopcuacore libopcuaserver
}
win32 {
    LIBS += -llibopcuaprotocol -llibopcuacore -llibopcuaserver
}

include(../../lib/freeopcua/include/opc.pri)


SOURCES += main.cpp 
