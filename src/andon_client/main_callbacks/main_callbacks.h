#include "client_rpcutility.h"
#include "ketcp_object.h"


#ifndef MAIN_CALLBACKS_H
#define MAIN_CALLBACKS_H

// main_lambdas name space
namespace ML{

    /*extern*/ ClientRpcUtility *MLserverRpc;

    //TODO наследовать KeObject  из TcpDevice
    template<class T>
    QList<T*> loadObjects(QVariant resp, QObject * parent=0){
        QList<T*> list;
        for(auto i:QJsonDocument::fromJson(resp.toString().toUtf8()).array()){
            if(!i.isObject()){
                qDebug() << i.toVariant() << "is not an json object";
                continue;
            }
            T * obj=new T(parent);
            list << obj;
            QJsonObject jsonObj=i.toObject();
            for (auto it = jsonObj.constBegin(); it != jsonObj.constEnd(); it++)
                obj->setProperty(it.key().toLatin1(), it.value().toVariant());
        }
        return list;
    }

    void printResp(QVariant resp){
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

    void productionProdused(ClientRpcUtility *MLserverRpc, KeTcpObject* keObject, const QString &ioName, const QVariant &value){
        if ((ioName.compare("inputCode")==0) && ((value.toInt()!=0))){
            MLserverRpc->Query2Json(QString(
                                 "SELECT DISTINCT DEVICE_NAME, MOLD_NAME FROM PRODUCTION_PART_PRODUSED (%1,%2)")
                                     .arg(value.toInt()).arg(keObject->property("ID_TCPDEVICE").toInt()),
                                 (std::function<void(QVariant)>)printResp);
        }
    }

    void loadKeObjects(ClientRpcUtility *MLserverRpc,QObject * parent=0){
        //ID_TCPDEVICE, TCPDEVICE_IP, PORT, DEVICE_TYPE, DEVICE_NAME,
        //AUX_PROPERTIES_LIST, PASS, LOGIN, STATION_ID, ENABLED
        MLserverRpc->Query2Json("SELECT * FROM TBL_TCPDEVICES "
                              "WHERE DEVICE_TYPE='KBX100' AND ENABLED=1",[=](QVariant resp){
            QList<KeTcpObject*> list;
            list << loadObjects<KeTcpObject>(resp,parent);
            for (auto keObject:list){
                QObject::connect(keObject,&KeTcpObject::IOEvent,[=](const QString &ioName, const QVariant &value){
                    productionProdused(MLserverRpc, keObject, ioName, value);});
                keObject->startConnecting();
            }
        });
    }
}
#endif // MAIN_CALLBACKS_H
