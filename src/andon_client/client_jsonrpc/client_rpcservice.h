#ifndef CLIENT_RPCSERVICE_H
#define CLIENT_RPCSERVICE_H

#include "qjsonrpcservice.h"

class ClientRpcService : public QJsonRpcService
{
    Q_OBJECT
    Q_CLASSINFO("serviceName", JSONRPC_CLIENT_SERVICENAME)
public:
    ClientRpcService(QObject *parent = 0):QJsonRpcService(parent){}
    ~ClientRpcService(){}
private:

signals:

public slots:
    bool isAlive(){return true;}
private:

};

#endif //CLIENT_RPCSERVICE_H
