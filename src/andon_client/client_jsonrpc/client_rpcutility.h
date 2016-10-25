#ifndef CLIENT_RPCUTILITY_H
#define CLIENT_RPCUTILITY_H

#include <QObject>
#include <QHostAddress>

#include "json/qjsonvalue.h"
#include "qjsonrpcsocket.h"
#include "qjsonrpcservice.h"
#include "qjsonrpcservicereply.h"
#include <QJsonDocument>
//#include <QtScript>
#include <QJSEngine>
#include <functional>
#include <QThread>

//QScriptEngine->QJSEngine
//QScriptValue->QJSValue

class QJsonRpcSocket;
class ClientRpcUtility : public QThread
{
    Q_OBJECT
public:
    ClientRpcUtility(QObject *parent = 0);
    void setserverip(QHostAddress newserveraddress);



public slots:
    void setEngine(QJSEngine *SharedEngine);
    void Call_server_proc(QString RemoteMethodName, QString InParameterList,
                          QObject * sender_obj=0, QString BackMethodName=0);
    void ServerExecute(QString RemoteMethodName, QVariantList InParameterList,
                        std::function<void(QVariant response)> functor=[] (QVariant response) { qDebug()<<"response"<<response; });
    void ServerExecute(QString RemoteMethodName, QVariantList InParameterList,
                        QJSValue scriptFunctor);
    void Query2Json(QString queryText, std::function<void(QVariant response)> functor);

signals:
    void error(QString errorString);
    void server_proc_reply(QString replyString);

private:
    QJsonRpcSocket *m_client;
    QHostAddress serveraddress;
    QJSEngine* engine;
protected:
    virtual void run(void);
};

#endif
