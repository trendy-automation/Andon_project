#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <QTimer>
#include <QMetaObject>
#include <QMetaProperty>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <functional>
#include <QApplication>
//#include "qjsonrpctcpserver.h"
//#include "watchdog.h"

template<class T>
QVariantMap cfGetProperties(T * obj,const QStringList &requested)
{
    QVariantMap objProperties;
    const QMetaObject *metaObj = T::metaObject();
    for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i)
        if (requested.contains(metaObj->property(i).name()) || requested.isEmpty())
            objProperties.insert(QString(metaObj->property(i).name()), metaObj->property(i).read(obj));
    return objProperties;
}

template<class T>
void cfSetProperties(T * obj,const QVariantMap &objProperties)
{
    const QMetaObject *metaObj = obj->metaObject();
    for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i)
        if (objProperties.contains(metaObj->property(i).name()))
            metaObj->property(i).write(obj,objProperties.value(metaObj->property(i).name()));
}

template<class T>
void cfListenPort(T * obj, int port, int interval, int delay,const std::function<void()>& functor=0) {
    QTimer *listenPortTimer = new QTimer(qApp);
    QObject::connect(listenPortTimer,&QTimer::timeout,[obj,port,listenPortTimer,interval,functor](){
            if (obj->listen(QHostAddress::AnyIPv4, port)) {
                if(functor)
                    functor();
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

template<class T>
T* cfGetObject(const QString &objectName)
{
    T *obj =qApp->findChild<T*>(objectName);
    if(obj)
        return obj;
    qDebug()<<QString("Object %1 not found in App!").arg(objectName);
    return 0;
}

static void printResp(QVariant resp){
    for(auto i:QJsonDocument::fromJson(resp.toString().toUtf8()).array()){
        if(!i.isObject()){
            qDebug() << i.toVariant() << "is not an json object";
            continue;
        }
        QStringList printList;
        for (auto o:QJsonObject(i.toObject()))
            printList << o.toString();
        qDebug() << printList.join(" ");
    }
}


//static void appStartWatchdog(int port)
//{
//    qDebug()<<"Start Watchdog";
//    Watchdog* watchdog = getObject<Watchdog>("watchdog");
////    watchdog = qApp->findChild<Watchdog*>("watchdog");
////    if(!watchdog){
////        qDebug()<<"Object watchdog not found in App!";
////        return;
////    }
//    if(!watchdog)
//        return;
////    WatchdogRpcService * watchdogRpcService = new WatchdogRpcService(qApp);
////    watchdogRpcService->setObjectName("andonRpcService");
//    QJsonRpcTcpServer * watchdogRpcServer = new QJsonRpcTcpServer(qApp);
////    watchdogRpcServer->setObjectName("watchdogRpcServer");
////    watchdogRpcServer->addService(watchdogRpcService);
//    watchdogRpcServer->addService(watchdog);
//    listenPort<QJsonRpcTcpServer>(watchdogRpcServer,port,3000,700,&watchdog->startProcess);
//}

#endif // COMMON_FUNCTIONS_H
