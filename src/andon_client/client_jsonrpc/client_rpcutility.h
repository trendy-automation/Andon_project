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
#include <QJSValue>
#include <functional>
//#include <QThread>

//QScriptEngine->QJSEngine
//QScriptValue->QJSValue

//Q_DECLARE_METATYPE (std::function<QVariant(QVariant)>)
//Q_DECLARE_SMART_POINTER_METATYPE(std::function<QVariant(QVariant)>)

class QJsonRpcSocket;
class ClientRpcUtility : public QObject
{
    Q_OBJECT

//    template<typename T> struct FunctorType
//    {
//        typedef std::function<T(QVariant)> Type ;
//    };
//    FunctorType<int>::Type Funcp = func<int> ;

public:
    ClientRpcUtility(QObject *parent = 0);
    void setserverip(QHostAddress newserveraddress);



public slots:
    void setEngine(QJSEngine *SharedEngine);
//    void Call_server_proc(QString RemoteMethodName, QString InParameterList,
//                          QObject * sender_obj=0, QString BackMethodName=0);
    void ServerExecute(QString RemoteMethodName, QVariantList InParameterList,
                        std::function<void(QVariant)> functor=0);
    QJsonRpcServiceReply *ServerExecuteRet(QString RemoteMethodName, QVariantList InParameterList);
    QVariant ServerExecuteJS(QString RemoteMethodName, QVariantList InParameterList,
                        QString scriptFunctor);
    void Query2Json(QString queryText, std::function<void(QVariant)> functor=0);
    void Query2JsonJS(QString queryText, QString scriptFunctor);
    QVariant evaluate(const QString &script);
    QVariant query(QString queryText);

signals:
    void error(QString errorString);
    void server_proc_reply(QString replyString);
    void server_reply(const QString &RemoteMethodName, const QVariantList &InParameterList, const QVariant &response);

private:
    QJsonRpcSocket *m_client;
    QHostAddress serveraddress;
    QJSEngine* engine;
//protected:
//    virtual void run(void);
};

#endif
