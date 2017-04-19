#include "server_rpcutility.h"

#include <QTcpSocket>
#include <QHostAddress>



ServerRpcUtility::ServerRpcUtility(QObject *parent)
    : QThread(parent),
      m_Server(0)
{
}

void ServerRpcUtility::setEngine(QJSEngine* SharedEngine)
{
    engine=SharedEngine;
}

void ServerRpcUtility::ClientExecute(QString RemoteMethodName, QVariantList InParameterList,
                              std::function<void(QVariant response)> functor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(serveraddress.toString(), JSONRPC_CLIENT_PORT);
    if (!socket->waitForConnected()) {
        qDebug() << "could not connect to server: " << socket->errorString();
        emit error(socket->errorString());
        return functor(QVariant());
    }
    m_Server = new QJsonRpcSocket(socket, this);
    QVariant arg[10];
    for (int i=0;i<9;++i)
        arg[i]=i<InParameterList.count()?InParameterList.at(i):QVariant();
    QJsonRpcServiceReply *reply = m_Server->invokeRemoteMethod(QString(JSONRPC_CLIENT_SERVICENAME).append(".").append(RemoteMethodName),
                                                               arg[0],arg[1],arg[2],arg[3],arg[4],arg[5],arg[6],arg[7],arg[8],arg[9]);
    QObject::connect(reply, &QJsonRpcServiceReply::finished, [=] () {
        if (!reply) {
            qDebug() << "invalid reply received";
            return functor(QVariant());
        }
        if (!reply->response().result().toVariant().isValid()) {
            qDebug() << "RpcTcp::ServerExecute: invalid response received!!!"
                     << reply->response().errorMessage();
            return functor(QVariant());
        }
        return functor(reply->response().result().toVariant());
    });
}

void ServerRpcUtility::ClientExecute(QString RemoteMethodName, QVariantList InParameterList,
                              QJSValue scriptFunctor)
{
    std::function<void(QVariant response)> functor;
    if(scriptFunctor.isCallable())
        functor = [scriptFunctor,this] (QVariant response) {
            QJSValue(scriptFunctor).call(
                    QJSValueList() << engine->toScriptValue(response));
        };
    else {
        qDebug()<<"Default scriptFunctor";
        functor=[] (QVariant response) { qDebug()<<"scriptFunctor is not a function."<<"response"<<response; };
    }
    ClientExecute(RemoteMethodName, InParameterList, functor);
}

void ServerRpcUtility::setserverip(QHostAddress newserveraddress)  {
    serveraddress = newserveraddress;
}

void ServerRpcUtility::run(void)
{


}

