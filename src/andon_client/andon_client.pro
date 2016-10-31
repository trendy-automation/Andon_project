include(../../andon_project.pri)
include(../shared_classes/qttelnet/qttelnet.pri)
#include(../shared_classes/ketcp_client/ketcp_client.pri)
include(../shared_classes/message_handler/message_handler.pri)
include(../shared_classes/single_apprun/single_apprun.pri)
include(clientwebinterface/clientwebinterface.pri)
include(../shared_classes/websocket/websocket.pri)
include(../shared_classes/qftp/qftp.pri)
include(client_jsonrpc/client_jsonrpc.pri)
include(client_interface/client_interface.pri)
include(widgetkeyboard/widgetkeyboard.pri)
include(broadcastreceiver/broadcastreceiver.pri)
include(plc_station/plc_station.pri)
include($${ANDNPRJ_LIBPATH}/qextserialport-1.2rc/src/qextserialport.pri)
include(opcua_client/opcua_client.pri)

DEFINES += DEBUG_NAME='\\"Client\\"'
DEFINES += APP_NAME='\\"CLIENT\\"'

CONFIG  += console

#QT      += script
#QT      += scripttools
QT      += concurrent
QT      += serialport
QT      += widgets
QT      += qml

TARGET = andon_client

TEMPLATE = app

SOURCES += main.cpp

RESOURCES += clientjs.qrc
