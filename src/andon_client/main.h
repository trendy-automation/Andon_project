#ifndef MAIN_H
#define MAIN_H
#include "client_rpcutility.h"
#include "client_rpcservice.h"
#include "websocketclientwrapper.h"
#include "websockettransport.h"
#include "clientwebinterface.h"
#include "qjsonrpctcpserver.h"
#include "qwebchannel.h"
#include <QtWebSockets/QWebSocketServer>
#include "qftp.h"
#include "serlock_manager.h"

#include <QMetaObject>

static void appExecuteQuery(const QString &queryText)
{
    ClientRpcUtility *serverRpc =qApp->findChild<ClientRpcUtility*>("serverRpc");
    if(!serverRpc){
        qDebug()<<"object serverRpc not found in App";
        return;
    }
    serverRpc->Query2Json(queryText);
}


template<class T>
QVariantMap getProperties(T * obj,const QStringList &requested)
{
    QVariantMap objProperties;
    const QMetaObject *metaObj = T::metaObject();
    for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i)
        if (requested.contains(metaObj->property(i).name()) || requested.isEmpty())
            objProperties.insert(QString(metaObj->property(i).name()), metaObj->property(i).read(obj));
    return objProperties;
}

template<class T>
void setProperties(T * obj,const QVariantMap &objProperties)
{
    const QMetaObject *metaObj = obj->metaObject();
    for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i)
        if (objProperties.contains(metaObj->property(i).name()))
            metaObj->property(i).write(obj,objProperties.value(metaObj->property(i).name()));
}


template<class T>
void listenPort(T * obj, int port, int interval, int delay) {
    QTimer *listenPortTimer = new QTimer(qApp);
    QObject::connect(listenPortTimer,&QTimer::timeout,[obj,port,listenPortTimer,interval](){
            if (obj->listen(QHostAddress::AnyIPv4, port)) {
                qDebug()<<QString("%1: %2 port opened").arg(obj->objectName()).arg(port);
                listenPortTimer->stop();
                listenPortTimer->deleteLater();
            } else {
                qDebug()<<QString("%1: Failed to open port %2").arg(obj->objectName()).arg(port);
                listenPortTimer->start(interval);
            }
    });
    listenPortTimer->start(delay);
}

void WebsocketInit(int websocketPort, ClientRpcUtility *serverRpc)
{
    QWebSocketServer *webSocketServer = new QWebSocketServer(QStringLiteral("QWebChannel server of client"), QWebSocketServer::NonSecureMode,qApp);
    webSocketServer->setObjectName("webSocketServer");
    listenPort<QWebSocketServer>(webSocketServer,websocketPort,3000,2000);
    WebSocketClientWrapper *clientWrapper = new WebSocketClientWrapper(webSocketServer,qApp);
    QWebChannel *channel = new QWebChannel(qApp);
    QObject::connect(clientWrapper, &WebSocketClientWrapper::clientConnected,
                     channel, &QWebChannel::connectTo);
    channel->registerObject(QStringLiteral("clientWeb"), serverRpc);
}

void TelnetKanbanDeclare(ClientRpcUtility *serverRpc, QtTelnet *telnetClient, QByteArray kanbanNumber,
                         QByteArray user, QByteArray pass, int idDevice)
{
    serverRpc->Query2Json(QString("SELECT LOG_KANBAN_ID "
                                                      "FROM PROD_KANBAN_BEGIN(%1,'%2')")
                                                      .arg(idDevice)
                                                      .arg(QString(kanbanNumber)),
                          [=](QVariant resp){
        QJsonDocument JsonDoc(QJsonDocument::fromJson(resp.toString().toUtf8()));
        QJsonArray JsonArr = JsonDoc.array();
        QJsonObject JsonObj;
        if (!JsonArr.isEmpty())
            JsonObj = JsonArr.at(0).toObject();
        if (JsonObj.contains("LOG_KANBAN_ID"))
            telnetClient->kanbanDeclare(JsonObj["LOG_KANBAN_ID"].toInt(),
                                        kanbanNumber,user,pass,idDevice);
        else
            qDebug() << "Wrong qeury." << resp.toString()
                     << " No LOG_KANBAN_ID parameter";
    });
}


static void appCreateObjects(QVariant resp)
{
    QJsonArray array = QJsonDocument::fromJson(resp.toString().toUtf8()).array();
    for (auto row = array.begin(); row != array.end(); row++) {
        QJsonObject jsonRow=row->toObject();
        if (jsonRow.contains("DEVICE_TYPE")) {
            if (jsonRow["DEVICE_TYPE"].toString()=="SHERLOCK") {
                SherlockManager * sm = new SherlockManager;
                setProperties(sm,jsonRow.toVariantMap());
            }
        }
    }
}

#endif // MAIN_H
