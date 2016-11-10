#include "opcua_client.h"

#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaMonitoredValue>
#include <QtOpcUa/QOpcUaMonitoredEvent>
#include <QtOpcUa/QOpcUaSubscription>
#include <QtOpcUa/QOpcUaNode>
#include <QtOpcUa/QOpcUaProvider>
#include <QtCore/QDebug>
#include <QtCore/QScopedPointer>


#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>

//#include <keep_alive.h>



//const QByteArray constObjName = "OBJECT_NAME";
const QByteArray constCodePropName = "PROPERTY_NAME";
const QByteArray constDevId = "TCPDEVICE_ID";
const QString constInputCode("inputCode");
const QByteArray constFullNodeName = "propertyName";

OpcUaClient::OpcUaClient(QObject *parent)
    : QObject(parent)
    ,m_pProvider(new QOpcUaProvider(this))
    , m_valueSubscription(0)
    //, m_eventSubscription(0)
{

    m_pClient = m_pProvider->createClient("freeopcua");

    if (!m_pClient) {
        qWarning() << "Could not initialize QtOpcUa plugin: freeopcua";
        exit(EXIT_FAILURE);
    }

    QObject::connect(this, &OpcUaClient::serverFound,[this](){
            qDebug()<<1;
            if(!m_pClient){
                qDebug()<<"Error - !m_pClient";
                m_pClient = m_pProvider->createClient("freeopcua");
            }
            qDebug()<<1.5;
//            if (!m_pClient->isConnected()){
                bool res = m_pClient->connectToEndpoint(QString("opc.tcp://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT));
//                for(auto ts:m_pClient->findChildren<QTcpSocket*>()){
//                    qDebug()<<"tcpSocket found"<<ts;
//                    keep_alive(ts);
//                }
                qDebug()<<2;
                if (!res) {
                    qWarning() << QString("Failed to connect to endpoint: opc.tcp://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT);
                    exit(EXIT_FAILURE);
                } else
                    m_valueSubscription = m_pClient->createSubscription(100);
//            }
//            if(!m_valueSubscription)
//                m_valueSubscription = m_pClient->createSubscription(100);
    });

    QObject::connect(m_pClient, &QOpcUaClient::connectedChanged,[this](bool connected){
//        qDebug()<<"connected changed"<<connected;
        if (connected) {
            qDebug()<<3;
            m_isConnected=true;
            qDebug()<<4;
            if(!m_valueSubscription)
                m_valueSubscription = m_pClient->createSubscription(100);
    //        m_eventSubscription = m_pClient->createSubscription(3000);
            QMapIterator<int,QString> nodes(m_nodesMap);
            qDebug()<<5;
            while (nodes.hasNext()) {
                qDebug()<<6;
                nodes.next();
                monitorNode(nodes.key(),nodes.value());
                qDebug()<<7;
            }
        }
        else
            qDebug()<<"connected changed"<<connected;
    });

    QTimer *connectTimer=new QTimer;
    QTcpSocket *testSocket=new QTcpSocket;
    QObject::connect(connectTimer,&QTimer::timeout,[=](){
//        qDebug()<<"OPCUA WATCHDOG search server";
        connectTimer->setInterval(OPCUA_INTERVAL);
            if (testSocket->bind(QHostAddress(OPCUA_IP),OPCUA_PORT)) {
                testSocket->close();
                m_isConnected=false;
                qDebug()<<"OPCUA WATCHDOG testSocket->bind";
            }
            else {
                if (!m_isConnected){
                    if (m_pClient->isConnected()) {
                        qDebug()<<"OPCUA WATCHDOG m_pClient->isConnected()";
                        m_pClient->disconnectFromEndpoint();
//                        while (!m_monitoredValues.isEmpty())
//                            m_monitoredValues.takeLast()->deleteLater();
//                        m_valueSubscription->deleteLater();
//                        m_valueSubscription=0;
                    }
                    qDebug()<<"OPCUA WATCHDOG server found";
//                    connectTimer->stop();
                    QTimer::singleShot(0,connectTimer,&QTimer::stop);
                    QTimer::singleShot(5000,this,//&OpcUaClient::serverFound);
                                       [testSocket,connectTimer,this](){
                        qDebug()<<0;
                        if (!testSocket->bind(QHostAddress(OPCUA_IP),OPCUA_PORT))
                            emit serverFound();
                        connectTimer->start();
                    });

//                    emit serverFound();
                }
//                else
//                    qDebug()<<"OPCUA WATCHDOG m_isConnected is true";
            }
   });
    connectTimer->start(0);
}

OpcUaClient::~OpcUaClient()
{
    delete m_pClient;
    delete m_pProvider;
//    delete m_inputCodeMonitorVal;
    delete m_valueSubscription;
    //delete m_eventSubscription;
    m_nodesMap.clear();
}

void OpcUaClient::monitorNode(int deviceId, const QString &objectName)
{
    QOpcUaNode* node = m_pClient->node(QString(objectName).prepend("ns=2;s="));    
    if(node){
//        QOpcUaSubscription *subscription = m_pClient->createSubscription(10000);
//        subscription->setProperty("nodename",node->name());
//        subscription->setObjectName("subscription_"+node->name().toLatin1());
//        QOpcUaMonitoredEvent *monitoredEvent=subscription->addEvent(node);
//        monitoredEvent->setParent(subscription);
//        monitoredEvent->setObjectName("event_"+node->name().toLatin1());
////        QOpcUaMonitoredEvent *monitoredEvent=new QOpcUaMonitoredEvent(node, m_valueSubscription,node);
////        QOpcUaMonitoredEvent *monitoredEvent=m_valueSubscription->addEvent(node);
//        monitoredEvent->setProperty("nodename",node->name());
//        monitoredEvent->setObjectName(node->name());
//        m_monitoredEvents.append(monitoredEvent);
////        m_monitoredEvents.append(m_pClient->createSubscription(100)->addEvent(node));
////        m_monitoredEvents.append(new QOpcUaMonitoredEvent(node, m_valueSubscription, this));
//        if (m_monitoredEvents.last()) {
//            qDebug()<<"Subscribe success to event"<<node->name()
//                    <<"m_monitoredEvents.last()"<<m_monitoredEvents.last();
//            //m_monitoredEvents.last()->setProperty("nodename",node->name());
//            QObject::connect(monitoredEvent /*m_monitoredEvents.last()*/, &QOpcUaMonitoredEvent::newEvent,
//                             this,&OpcUaClient::processEvent);
////            QObject::connect(m_monitoredEvents.last(), &QOpcUaMonitoredEvent::newEvent,
////                             [monitoredEvent](QVector<QVariant> value){
////                if(monitoredEvent)
////                    if (monitoredEvent->property("nodename")==monitoredEvent->parent()->property("nodename"))
////                        qDebug() << monitoredEvent->objectName()
////                             << monitoredEvent->property("nodename").toString()
////                             << value;
////                    else
////                        qDebug() << "Wrong event" ;
//////                QOpcUaMonitoredEvent *sender = qobject_cast<QOpcUaMonitoredEvent *>(QObject::sender());
//////                if(sender) {
//////                    qDebug()<<sender->objectName()<<value;
//////                }
////            });

//        }
//        else
//            qDebug()<<"Failed subscribe to event"<<node->name();

        QTimer *subscribeTimer=new QTimer;
        QObject::connect(subscribeTimer, &QTimer::timeout,
                         [this,subscribeTimer,node,deviceId](){
            subscribeTimer->setInterval(1800000);
            QStringList properties = node->childIds();
            //TODO all fields to constant string (inputCode)
            //TODO tag system in DB
            QListIterator<QString> prop(properties);
//            if(properties.count()!=node->property("propertiesCount").toInt()){
//                node->setProperty("propertiesCount",properties.count());
                qDebug()<<node<<node->name()<<properties.count()<<"properties";
//            }
    //        for(auto id:node->childIds())
            while (prop.hasNext()) {
                QString propertyName(prop.next());
                if(propertyName.split(".").last()==constInputCode){
                    QOpcUaNode* propertyNode = m_pClient->node(propertyName);
                    propertyNode->setProperty(constDevId,deviceId);
                    propertyNode->setProperty(constFullNodeName,propertyName);
                    subscribeProperty(propertyNode);
                }
                // else qDebug()<<propertyName<<"not subscribe";
            }
        });
        subscribeTimer->start(0);

    }
    else
        qDebug()<<"Can not finde object"<<objectName;
}

bool OpcUaClient::subscribeProperty(QOpcUaNode *property)
{
    QString nodeName = property->property(constFullNodeName).toString();
    if (property) {
            qDebug()<<8;
            QOpcUaMonitoredValue *oldMonitoredValue=m_valueSubscription->findChild<QOpcUaMonitoredValue*>(
                        nodeName);
            if (oldMonitoredValue) {
                qDebug()<<nodeName<<oldMonitoredValue->node().value();
                return true;
            }

            QOpcUaMonitoredValue *monitoredValue=m_valueSubscription->addValue(property);
            qDebug()<<9;
            if (monitoredValue) {
                monitoredValue->setObjectName(nodeName);
                //monitoredValue->setParent(m_valueSubscription);
                monitoredValue->setProperty(constDevId,property->property(constDevId));
                monitoredValue->setProperty(constCodePropName,property->name());
                qDebug()<<"Subscribe success to value"<<nodeName<<property<<monitoredValue;
                QObject::connect(monitoredValue, &QOpcUaMonitoredValue::valueChanged,
//                        this,&OpcUaClient::processValue);
                                 [this,monitoredValue](QVariant value){
                    emit propertyChanged(monitoredValue->property(constDevId).toInt(),
                                         monitoredValue->property(constCodePropName).toString(),value);
                });
//                QTimer *monTimer=new QTimer;
//                QObject::connect(monTimer, &QTimer::timeout,
//                                 [monitoredValue](){
//                    qDebug()<<monitoredValue->property(constCodePropName).toString()<<"dumpObjectInfo";
// //                            <<"receivers"<<monitoredValue->receivers("valueChanged")
//                    monitoredValue->dumpObjectInfo();
//                });
//                monTimer->start(1800000);
                return true;
            }
//            else
//                m_monitoredValues.removeLast();
        }
    qDebug()<<"Can not subscribe to"<<nodeName;
    return false;
}

void OpcUaClient::processValue(QVariant value)
{
    QOpcUaMonitoredValue *sender = qobject_cast<QOpcUaMonitoredValue *>(QObject::sender());
//    qDebug()<<"valueChanged"<<value
//                            <<sender->property(constDevId).toInt()
//                            <<sender->property(constCodePropName).toString();
    if(sender) {
//        qDebug()<<"sender qobject_cast OK"<<sender->property(constDevId).toInt()
//                <<sender->property(constCodePropName).toString()<<value;
        emit propertyChanged(sender->property(constDevId).toInt(),
                             sender->property(constCodePropName).toString(),value);
    }

}

void OpcUaClient::processEvent(QVector<QVariant> value)
{
    QOpcUaMonitoredEvent *sender = qobject_cast<QOpcUaMonitoredEvent *>(QObject::sender());
    if(sender) {
        qDebug() << sender->parent() << sender
                 << sender->parent()->property("nodename").toString()
                 << sender->property("nodename").toString()
                 << sender->parent()->objectName() << sender->objectName();
//        if (sender->property("nodename")==sender->parent()->property("nodename"))
//                                qDebug() << sender->objectName()
//                                     << sender->property("nodename").toString()
//                                     << value;
//                            else
//                                qDebug() << "Wrong event" ;
    }
}

    void OpcUaClient::updateValue(QVector<QVariant> value)
{
    qDebug()<<value;
    //TODO feedback
}


void OpcUaClient::appendObject(int deviceId, const QString &objectName)
{
    m_nodesMap.insert(deviceId,objectName);
//    if (m_pClient->isConnected())
    if (m_isConnected)
        monitorNode(deviceId,objectName);
}
