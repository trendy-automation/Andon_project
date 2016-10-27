#ANDNPRJ_LIBPATH = ../../../lib
include($${ANDNPRJ_LIBPATH}/qextserialport-1.2rc/src/qextserialport.pri)

#QT       += core
#QT       -= gui

#CONFIG += console
#CONFIG -= app_bundle

INCLUDEPATH+=$$PWD
HEADERS += $$PWD/sms_service.h
SOURCES += $$PWD/sms_service.cpp


