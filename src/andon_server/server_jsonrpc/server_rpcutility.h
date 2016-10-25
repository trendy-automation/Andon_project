#ifndef SERVER_RPCUTILITY_H
#define SERVER_RPCUTILITY_H

#include <QObject>
#include <QHostAddress>

#include "json/qjsonvalue.h"
#include "qjsonrpcsocket.h"
#include "qjsonrpcservice.h"
#include "qjsonrpcservicereply.h"
#include <QJsonDocument>

//#include <QtScript>
#include <QJSEngine>
using namespace std;
#include <functional>
#include <QThread>

//#include <thread>

//void foo(std::function< void (void) > f) {
//  f();
//}

class QJsonRpcSocket;
//class QString;
class ServerRpcUtility : public QThread
{
    Q_OBJECT
public:
    ServerRpcUtility(QObject *parent = 0);
    void setserverip(QHostAddress newserveraddress);



public slots:
    void setEngine(QJSEngine *SharedEngine);
    void ServerExecute(QString RemoteMethodName, QVariantList InParameterList,
                        std::function<void(QVariant response)> functor=[] (QVariant response) { qDebug()<<"response"<<response; });
    void ServerExecute(QString RemoteMethodName, QVariantList InParameterList,
                        QJSValue scriptFunctor);
signals:
    void error(QString errorString);
    void server_proc_reply(QString replyString);

private:
    QJsonRpcSocket *m_Server;
    QHostAddress serveraddress;
    QJSEngine* engine;
protected:
    virtual void run(void);
};

#endif
