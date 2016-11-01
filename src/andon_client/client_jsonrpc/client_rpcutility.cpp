#include <QTcpSocket>
#include "client_rpcutility.h"
#include <QHostAddress>
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

void ClientRpcUtility::Call_server_proc(QString RemoteMethodName,QString InParameterList, QObject * sender_obj, QString BackMethodName)
{
    //QObject *sender = static_cast<QObject *>(sender());
    //TODO: get Sender
    //qDebug() << "Call_server_proc"<<RemoteMethodName<<InParameterList;
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(serveraddress.toString(), JSONRPC_SERVER_PORT);
    if (!socket->waitForConnected()) {
        qDebug() << "could not connect to server: " << socket->errorString();
        emit error(socket->errorString());
        //TODO: block buttons
        return;
    }
    m_client = new QJsonRpcSocket(socket, this);
    //qDebug() << RemoteMethodName << InParameterList.toUtf8();
    QJsonRpcServiceReply *reply = m_client->invokeRemoteMethod(QString(JSONRPC_SERVER_SERVICENAME).append(".").append(RemoteMethodName), InParameterList);

    //QObject::connect(reply, &QJsonRpcServiceReply::finished, this, &ClientRpcUtility::processRetResponse);
    //qDebug() << "connect reply->"<< sender_obj->objectName() << BackMethodName;
    QObject::connect(reply, &QJsonRpcServiceReply::finished,
                     [reply,this,sender_obj,BackMethodName] () {
        if (!reply) {
            qDebug() << "invalid reply received";
            return;
        }
        if (!reply->response().isValid()) {
            qDebug() << "invalid response received";
            return;
        }

        //qDebug() << reply->response().result().toString();
        //qDebug() << sender_obj->objectName() << BackMethodName;
        QJsonDocument jsonresult=QJsonDocument::fromJson(reply->response().result().toString().toUtf8());
        if (sender_obj && !BackMethodName.isEmpty()){
            QGenericArgument result=Q_ARG(QJsonDocument,jsonresult);
            QMetaObject::invokeMethod(sender_obj,BackMethodName.toLatin1(), result);
        }
        else emit server_proc_reply(reply->response().result().toString()); //.replace("\\",""));
    });
    //qDebug() << "reply connected";

}
void ClientRpcUtility::ServerExecute(QString RemoteMethodName, QVariantList InParameterList,
                              std::function<void(QVariant response)> functor)
{
    qDebug() << "ServerExecute"<<RemoteMethodName<<InParameterList;
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(serveraddress.toString(), JSONRPC_SERVER_PORT);
    if (!socket->waitForConnected()) {
        qDebug() << "could not connect to server: " << socket->errorString();
        emit error(socket->errorString());
        return functor(QVariant());
    }
    m_client = new QJsonRpcSocket(socket, this);
    QVariant arg[10];
    for (int i=0;i<9;++i)
        arg[i]=i<InParameterList.count()?InParameterList.at(i):QVariant();
    QJsonRpcServiceReply *reply = m_client->invokeRemoteMethod(QString(JSONRPC_SERVER_SERVICENAME).append(".").append(RemoteMethodName),
                                                               arg[0],arg[1],arg[2],arg[3],arg[4],arg[5],arg[6],arg[7],arg[8],arg[9]);
    qDebug() << "invokeRemoteMethod";
    QObject::connect(reply, &QJsonRpcServiceReply::finished, [=] () {
        if (!reply) {
            qDebug() << "invalid reply received";
            return functor(QVariant());
        }
        if (!reply->response().result().toVariant().isValid()) {
            qDebug() << "invalid response received!!!"
                     << reply->response().errorMessage();
            return functor(QVariant());
        }
        //qDebug() << "ServerExecute return functor";
        qDebug() << reply->response().result().toVariant();
        return functor(reply->response().result().toVariant());
    });
}

void ClientRpcUtility::ServerExecuteJS(QString RemoteMethodName, QVariantList InParameterList,
                              QJSValue scriptFunctor)
{
    qDebug() << "ServerExecute scriptFunctor";
    std::function<void(QVariant response)> functor;
    if(engine && scriptFunctor.isCallable()){
//        qDebug() << "scriptFunctor callable";
        functor = [scriptFunctor,this] (QVariant response) {
                QJSValue result = QJSValue(scriptFunctor).call(
                        QJSValueList() << engine->toScriptValue(response));
                if (result.isError())
                    qDebug() << "Uncaught exception at line"
                             << result.property("lineNumber").toInt()
                             << ":" << result.toString();
                else {
                    if (result.isVariant()){
                        qDebug()<<result.toVariant();
                        this->setProperty("result",result.toVariant());
                    }
                }
        };
    }
    else {
        qDebug()<<"Default scriptFunctor";
        functor=[] (QVariant response) { qDebug()<<"scriptFunctor is not a function."<<response; };
    }
    ServerExecute(RemoteMethodName, InParameterList, functor);
}

void ClientRpcUtility::Query2Json(QString queryText, std::function<void(QVariant response)> functor)
{
    return ServerExecute("SQLQuery2Json", QVariantList()<<queryText, functor);
}

void ClientRpcUtility::Query2JsonJS(QString queryText, QJSValue scriptFunctor)
{
    qDebug()<<"scriptFunctor"<<scriptFunctor.toString();
    return ServerExecuteJS("SQLQuery2Json", QVariantList()<<queryText, scriptFunctor);
}

void ClientRpcUtility::setserverip(QHostAddress newserveraddress)  {
    serveraddress = newserveraddress;
}

//void ClientRpcUtility::run(void)
//{

//}

QVariant ClientRpcUtility::evaluate(const QString &script)
{
    if(engine){
        QJSValue result = engine->evaluate(script);
        if (result.isError())
            qDebug() << "Uncaught exception at line"
                     << result.property("lineNumber").toInt()
                     << ":" << result.toString();
        qDebug()<<result.toVariant();
        if (result.isVariant())
            return result.toVariant();
    }
    return QVariant();
}



