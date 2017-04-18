#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <QTimer>
#include <QMetaObject>
#include <QJsonDocument>
#include <functional>

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
bool listenPort(T * obj, int port, int interval, int delay,const std::function<void()>& functor=0) {
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

#endif // COMMON_FUNCTIONS_H
