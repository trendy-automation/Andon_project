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
QT      += concurrent
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
#CONFIG += console
#CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    #$$PWD/jsonrpc_tcpserver/testservice.cpp \
    #$$PWD/sms_service/sms_service.cpp \
    #$$PWD/broadcastsender/sender.cpp #\

#HEADERS += \
    #$$PWD/jsonrpc_tcpserver/testservice.h \
    #$$PWD/sms_service/sms_service.h \
    #$$PWD/sql_fb/sql_fb.h \
    #$$PWD/broadcastsender/sender.h \
    #$$PWD/iostreamtheard/iostreamtheard.h



#INCLUDEPATH += jsonrpc_tcpserver \
#               sms_service \
#               sql_fb \
#               broadcastsender #\
               #iostreamthread
               #webui

# Location of SMTP Library
#SMTP_LIBRARY_LOCATION = $$PWD/../../../build-andon_project-Desktop_Qt_5_4_0_MinGW_32bit-Release/src/andon_server/

#win32:CONFIG(release, debug|release): LIBS += -L$$SMTP_LIBRARY_LOCATION/release/ -lSMTPEmail
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$SMTP_LIBRARY_LOCATION/debug/ -lSMTPEmail
#else:unix: LIBS += -L$$SMTP_LIBRARY_LOCATION -lSMTPEmail

#INCLUDEPATH += $$SMTP_LIBRARY_LOCATION
#DEPENDPATH += $$SMTP_LIBRARY_LOCATION


