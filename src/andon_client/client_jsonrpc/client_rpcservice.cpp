#include "client_rpcservice.h"
#include "qjsonrpcsocket.h"




ClientRpcService::ClientRpcService(QObject *parent)
    : QJsonRpcService(parent)
{

}

ClientRpcService::~ClientRpcService()
{
}


