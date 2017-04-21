#ifndef WATCHDOG_RPCSERVICE_H
#define WATCHDOG_RPCSERVICE_H
#include "qjsonrpcservice.h"
class WatchdogRpcService : public QJsonRpcService
{
    Q_OBJECT
    Q_CLASSINFO("serviceName", JSONRPC_WATCHDOG_SERVICENAME)
public:
    WatchdogRpcService(QObject *parent = 0):QJsonRpcService(parent){}
    ~WatchdogRpcService(){}
public slots:
    bool isAlive(){return true;}
};
#endif // WATCHDOG_RPCSERVICE_H
