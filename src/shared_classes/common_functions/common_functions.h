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
#include <QThread>
#include <QHostAddress>
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
void cfListenPort(T * obj, ushort port, int interval, int delay,const std::function<void()>& functor=0) {
    QTimer *listenTimer = new QTimer(0);
    //qDebug()<<0;
//    qDebug()<<"obj->thread().isRunning()"<<obj->thread()->isRunning();
//    QThread* thread = new QThread;
//    if(!obj->parent()){
//        qDebug()<<"thread";
//        obj->moveToThread(thread);
//        listenTimer->moveToThread(thread);
//        listenTimer->setInterval(delay);
//    } else{
//        qDebug()<<"listenTimer";
//        listenTimer->start(delay);
//    }
//    QObject::connect(thread,&QThread::started,listenTimer,static_cast<void (QTimer::*)()>(&QTimer::start));
//    thread->start();

    //listenTimer->moveToThread(obj->thread());
    //QMetaObject::invokeMethod(listenTimer, "start", Qt::QueuedConnection, Q_ARG(int, delay));

    QObject::connect(listenTimer,&QTimer::timeout,obj,[obj,port,listenTimer,interval,functor](){
//        QTimer::singleShot(0,[obj,port,listenTimer,interval,functor](){
//            qDebug()<<1;
//            bool isPortOpen;
//            QMetaObject::invokeMethod(obj, "listen", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, isPortOpen),
//                                      Q_ARG(QHostAddress, QHostAddress::AnyIPv4), Q_ARG(ushort, port));
            if (obj->listen(QHostAddress::AnyIPv4, port)) {
//            if(isPortOpen){
//                qDebug()<<2;
                if(functor)
                    functor();
                qDebug()<<QString("%1: %2 port opened").arg(obj->objectName()).arg(port);
                listenTimer->stop();
                listenTimer->deleteLater();
            } else {
                qDebug()<<QString("%1: Failed to open port %2").arg(obj->objectName()).arg(port);
                listenTimer->start(interval);
            }
//        });
    }, Qt::QueuedConnection);
    listenTimer->start(delay);
    //QTimer::singleShot(0,Qt::CoarseTimer,obj,[](){qDebug()<<3;}/*, Qt::QueuedConnection*/);
    //QTimer::singleShot(0,Qt::CoarseTimer,obj,[listenTimer,delay](){listenTimer->start(delay);}/*, Qt::QueuedConnection*/);
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
