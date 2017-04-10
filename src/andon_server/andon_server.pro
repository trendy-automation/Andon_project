include(../../andon_project.pri)
include(../shared_classes/qttelnet/qttelnet.pri)
include(../shared_classes/iostreamtheard/iostreamtheard.pri)
include(../shared_classes/dbwrapper/dbwrapper.pri)
include(../shared_classes/message_handler/message_handler.pri)
include(../shared_classes/single_apprun/single_apprun.pri)
include(server_jsonrpc/server_jsonrpc.pri)
include(webui/webui.pri)
include(sms_service/sms_service.pri)
include(broadcastsender/bcsender.pri)
include(qtxlsx/qtxlsx.pri)
include(sendemail/sendemail.pri)

DEFINES += DEBUG_NAME='\\"Server\\"'
DEFINES += APP_NAME='\\"SERVER\\"'

#SMTPCLIENTDESTDIR= $${DESTDIR}/../lib/SmtpClient/release
#QJSONRPCDESTDIR= $${DESTDIR}/../lib/qjsonrpc/release

#include($${ANDNPRJ_LIBPATH}/qjsonrpc/qjsonrpc.pri)
#INCLUDEPATH += $${ANDNPRJ_LIBPATH}/qjsonrpc/src \
#               $${ANDNPRJ_LIBPATH}/qjsonrpc/src/json
#LIBS += -L$${ANDNPRJ_LIBPATH}/qjsonrpc/src $${QJSONRPC_LIBS}
#QT += core network testlib

#include sms_service
#include($${ANDNPRJ_LIBPATH}/qextserialport-1.2rc/src/qextserialport.pri)
#QT       += core
#QT       -= gui

#QT      += script
#QT      += scripttools
#QT      += concurrent
QT      += gui
QT      += widgets
QT      += qml

CONFIG += console
CONFIG -= app_bundle
#CONFIG += c++14

#include sql_fb
#QT       += core
#QT       += sql
#QT       -= gui
#QT       += sql widgets widgets
#QT       += sql widgets
#CONFIG   += console
#CONFIG   -= app_bundle




TARGET = andon_server

TEMPLATE = app

SOURCES += main.cpp \

