CONFIG  += c++14

ANDNPRJ_VERSION = 2.2
ANDNPRJ_INCLUDEPATH = $${PWD}/inc
ANDNPRJ_LIBPATH = $${PWD}/lib

DEFINES += QT_MESSAGELOGCONTEXT

DEFINES += APP_VER='\\"$${ANDNPRJ_VERSION}\\"'
#______________SERVER_______________
#_______________ upd _______________
DEFINES += UDP_SRVRNAME='\\"Andon\040server\040ver.\0402.2\\"'
DEFINES += UDP_PORT=45455
DEFINES += UDP_INTERVAL=3000

#_______________ qjsonrpc _______________
DEFINES += JSONRPC_SERVER_PORT=40001
DEFINES += JSONRPC_CLIENT_PORT=40002
DEFINES += JSONRPC_SERVER_SERVICENAME='\\"serverRPC\\"'
DEFINES += JSONRPC_CLIENT_SERVICENAME='\\"clientRPC\\"'

#_______________ sendemail _______________
DEFINES += EMAIL_DOMEN='\\"AD-CORP\\"' #LS
DEFINES += EMAIL_AUTH=false
DEFINES += EMAIL_HOST='\\"EUEDCEXC0041.EDC.EU.CORP\\"' #10.20.114.141
DEFINES += EMAIL_PORT=25
DEFINES += EMAIL_SSL=false
DEFINES += EMAIL_ADDRESS='\\"kalugafis-andon@faurecia.com\\"'
DEFINES += EMAIL_USER='\\"7andonka\\"'
DEFINES += EMAIL_PASS='\\"And0n2016\\"'

#_______________ dbwarpper _______________
DEFINES += DATABASE_FILE='\\"ANDON_DB.FDB\\"'

#_______________ webui _______________
DEFINES += WUI_PATH='\\"./webui2\\"'
DEFINES += WUI_PORT=8081
DEFINES += WUI_CLAEN_INTERVAL=900000 #each 15 minutes
DEFINES += WUI_UPDATE_INTERVAL=15000 #each 15 SECONDS
DEFINES += WEBCHANEL_PORT=12346


#______________ CLIENT _______________
#ANDNPRJ_LIBS = -lkeyboardwidget
#______________ FTP _______________
DEFINES += FTP_INTERVAL=10800000

#TODO add broadcast sender/resiver
#______________ OPCUA _______________
DEFINES += OPCUA_IP='\\"localhost\\"'
DEFINES += OPCUA_PORT=43344
DEFINES += OPCUA_INTERVAL=3000
