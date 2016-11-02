#include <QTcpSocket>
#include "client_rpcutility.h"
#include <QHostAddress>
#include <QEventLoop>
#include <QApplication>

//TODO QThread
ClientRpcUtility::ClientRpcUtility(QObject *parent)
    : QObject(parent),
      m_client(0)
{
}

void ClientRpcUtility::setEngine(QJSEngine* SharedEngine)
{
    engine=SharedEngine;
}

void ClientRpcUtility::ServerExecute(QString RemoteMethodName, QVariantList InParameterList,
                              std::function<void(QVariant)> functor)
{
    qDebug()<<RemoteMethodName<<InParameterList;
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(serveraddress.toString(), JSONRPC_SERVER_PORT);
    if (!socket->waitForConnected()) {
        qDebug() << "could not connect to server: " << socket->errorString();
        emit error(socket->errorString());
        //return functor(QVariant());
        functor(QVariant());
    }
    m_client = new QJsonRpcSocket(socket, this);
    QVariant arg[10];
    for (int i=0;i<9;++i)
        arg[i]=i<InParameterList.count()?InParameterList.at(i):QVariant();
    QJsonRpcServiceReply *reply = m_client->invokeRemoteMethod(QString(JSONRPC_SERVER_SERVICENAME).append(".").append(RemoteMethodName),
                                                               arg[0],arg[1],arg[2],arg[3],arg[4],arg[5],arg[6],arg[7],arg[8],arg[9]);
//    qDebug() << "invokeRemoteMethod";
    QObject::connect(reply, &QJsonRpcServiceReply::finished, [=] () {
        if (!reply) {
            qDebug() << "invalid reply received";
            //return functor(QVariant());
            functor(QVariant());
        }
        if (!reply->response().result().toVariant().isValid()) {
            qDebug() << "invalid response received!!!"
                     << reply->response().errorMessage();
            //return functor(QVariant());
            functor(QVariant());
        }
        //qDebug() << "ServerExecute return functor";
        qDebug() << reply->response().result().toVariant();
        if(functor==0){
//            return [] (QVariant response) {qDebug()<<"response"<<response;}
            qDebug() << "emit server_reply";
            emit server_reply(RemoteMethodName, InParameterList, reply->response().result().toVariant());
//            return QVariant();
        }
        else
            return functor(reply->response().result().toVariant());
    });
}

QJsonRpcServiceReply *ClientRpcUtility::ServerExecuteRet(QString RemoteMethodName, QVariantList InParameterList)
{
    qDebug() << RemoteMethodName<<InParameterList;
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(serveraddress.toString(), JSONRPC_SERVER_PORT);
    if (!socket->waitForConnected()) {
        qDebug() << "could not connect to server: " << socket->errorString();
        emit error(socket->errorString());
        return 0;
    }
    m_client = new QJsonRpcSocket(socket, this);
    QVariant arg[10];
    for (int i=0;i<9;++i)
        arg[i]=i<InParameterList.count()?InParameterList.at(i):QVariant();
    QJsonRpcServiceReply *reply = m_client->invokeRemoteMethod(QString(JSONRPC_SERVER_SERVICENAME).append(".").append(RemoteMethodName),
                                                               arg[0],arg[1],arg[2],arg[3],arg[4],arg[5],arg[6],arg[7],arg[8],arg[9]);
    return reply;
}


QVariant ClientRpcUtility::ServerExecuteJS(QString RemoteMethodName, QVariantList InParameterList,
                              QString scriptFunctor)
{
    qDebug() << "scriptFunctor";
    std::function<QVariant(QVariant response)> functor;
    if(engine){ // && scriptFunctor.isCallable()
//        qDebug() << "scriptFunctor callable";
        functor = [scriptFunctor,this] (QVariant response)->QVariant{
                QJSValue result = engine->evaluate(scriptFunctor).call( //scriptFunctor.call(
                        QJSValueList() << engine->toScriptValue(response));
                if (result.isError())
                    qDebug() << "Uncaught exception at line"
                             << result.property("lineNumber").toInt()
                             << ":" << result.toString();
                else {
                    if (result.isVariant()){
                        qDebug()<<result.toVariant();
                        return result.toVariant();
//                        this->setProperty("result",result.toVariant());
                    }
                }
        };
    }
    else {
        qDebug()<<"Default scriptFunctor";
        functor=[] (QVariant response)->QVariant{ qDebug()<<"scriptFunctor is not a function."<<response; };
    }
    ServerExecute(RemoteMethodName, InParameterList, functor);
}

void ClientRpcUtility::Query2Json(QString queryText, std::function<void(QVariant)> functor)
{
    ServerExecute("SQLQuery2Json", QVariantList()<<queryText, functor);
}

void ClientRpcUtility::Query2JsonJS(QString queryText, QString scriptFunctor)
{
    qDebug()<<"scriptFunctor"<<scriptFunctor;
    ServerExecuteJS("SQLQuery2Json", QVariantList()<<queryText, scriptFunctor);
}

void ClientRpcUtility::setserverip(QHostAddress newserveraddress)  {
    serveraddress = newserveraddress;
}

QVariant ClientRpcUtility::evaluate(const QString &script)
{
    if(engine){
        QJSValue result = engine->evaluate(script);
        if (result.isError())
            qDebug() << "Uncaught exception at line"
                     << result.property("lineNumber").toInt()
                     << ":" << result.toString()
                     << "in script:" << script;
                        qDebug()<<result.toVariant();
        if (result.isVariant())
            return result.toVariant();
    }
    return QVariant();
}

QVariant ClientRpcUtility::query(QString queryText)
{
    QEventLoop replyWaitLoop;
    QJsonRpcServiceReply * reply = ServerExecuteRet("SQLQuery2Json", QVariantList()<<queryText);
    QObject::connect(reply, &QJsonRpcServiceReply::finished, &replyWaitLoop, &QEventLoop::quit);
//    QObject::connect(reply, &QJsonRpcServiceReply::destroyed, &replyWaitLoop, &QEventLoop::quit);
    if (reply){
        replyWaitLoop.exec();
        if (!reply->response().result().toVariant().isValid())
            qDebug() << "invalid response received!!!"
                     << reply->response().errorMessage();
        else
            return reply->response().result().toVariant();
    }
    return QVariant();
}




