QT       += core network

#____________ TELNET SAP _______________
DEFINES += TELNET_LOGIN='\\"SAPcoRPC\\"'
DEFINES += TELNET_PASS='\\"Faurecia01*\\"'
DEFINES += DEF_TASK_TIMEOUT=30

SAP_SYSTEM=FC1
DEFINES += TELNET_SAP_SYSTEM='\\"$${SAP_SYSTEM}\\"'
equals(SAP_SYSTEM,PC1) {
    DEFINES += TELNET_HOST='\\"10.20.178.105\\"' #PC1
    DEFINES += SAP_DEF_LOGIN='\\"RUTYABC019\\"'  #PC1
    DEFINES += SAP_DEF_PASS='\\"initial\\"'    #PC1
#    DEFINES += SAP_DEF_LOGIN='\\"RUTYABC020\\"'  #PC1
#    DEFINES += SAP_DEF_PASS='\\"Initial2@\\"'    #PC1
} else {
    DEFINES += TELNET_HOST='\\"10.224.29.60\\"' #FC1
    DEFINES += SAP_DEF_LOGIN='\\"RUTYABC024\\"' #FC1
    DEFINES += SAP_DEF_PASS='\\"initial\\"'     #FC1
}


INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qttelnet.h
SOURCES += \
    $$PWD/qttelnet.cpp

#win32:LIBS += -lws2_32
