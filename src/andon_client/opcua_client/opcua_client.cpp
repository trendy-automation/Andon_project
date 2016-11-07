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


//const QByteArray constObjName = "OBJECT_NAME";
const QByteArray constPropName = "PROPERTY_NAME";
const QByteArray constDevId = "TCPDEVICE_ID";
const QString constInputCode("inputCode");

OpcUaClient::OpcUaClient(QObject *parent)
    : QObject(parent)
    ,m_pProvider(new QOpcUaProvider(this))
    , m_valueSubscription(0)
    , m_eventSubscription(0)
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
            bool res = m_pClient->connectToEndpoint(QString("opc.tcp://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT));
            qDebug()<<2;
            if (!res) {
                qWarning() << QString("Failed to connect to endpoint: opc.tcp://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT);
                exit(EXIT_FAILURE);
            }
            if(!m_valueSubscription)
                m_valueSubscription = m_pClient->createSubscription(100);
    });

    QObject::connect(m_pClient, &QOpcUaClient::connectedChanged,[this](bool connected){
//        qDebug()<<"connected changed"<<connected;
        if (connected) {
            qDebug()<<3;
            m_isConnected=true;
            qDebug()<<4;
//            m_valueSubscription = m_pClient->createSubscription(100);
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
//        qDebug()<<"OPCUA search server";
        connectTimer->setInterval(OPCUA_INTERVAL);
            if (testSocket->bind(QHostAddress(OPCUA_IP),OPCUA_PORT)) {
                testSocket->close();
                m_isConnected=false;
            }
            else {
                if (!m_isConnected){
                    if (m_pClient->isConnected()) {
//                        qDebug()<<"m_monitoredValues m_valueSubscription deleteLater";
                        m_pClient->disconnectFromEndpoint();
                        while (!m_monitoredValues.isEmpty())
                            m_monitoredValues.takeLast()->deleteLater();
                        m_valueSubscription->deleteLater();
                        m_valueSubscription=0;
                    }
                    qDebug()<<"OPCUA server found";
                    connectTimer->stop();
                    QTimer::singleShot(5000,this,&OpcUaClient::serverFound);
//                                       [testSocket,this](){
//                        qDebug()<<0;
//                        if (!testSocket->bind(QHostAddress(OPCUA_IP),OPCUA_PORT))
//                            emit serverFound();
//                    });

                    //emit serverFound();
                }
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
    delete m_eventSubscription;
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




        QStringList properties = node->childIds();
        //TODO all fields to constant string (inputCode)
        //TODO tag system in DB
        QListIterator<QString> prop(properties);
        qDebug()<<node->name()<<properties.count()<<"properties";
        while (prop.hasNext()) {
            QString propertyName(prop.next());
            if(propertyName.split(".").last()==constInputCode){
                QOpcUaNode* propertyNode = m_pClient->node(propertyName);
                propertyNode->setProperty(constDevId,deviceId);
                subscribeProperty(propertyNode);
            }
            // else qDebug()<<propertyName<<"not subscribe";
        }
    }
    else
        qDebug()<<"Can not finde object"<<objectName;
}

bool OpcUaClient::subscribeProperty(QOpcUaNode *property)
{
//    QString *nodeName= new QString(node->name());
    if (property) {
        QOpcUaMonitoredEvent *monitoredEvent = m_valueSubscription->addEvent(property);
        if (monitoredEvent) {
            qDebug()<<"Subscribe success to event"<<property->name();
            QObject::connect(monitoredEvent, &QOpcUaMonitoredEvent::newEvent,
                             [=](QVector<QVariant> value){qDebug()<<property->name()<<value;});
        }
        else
            qDebug()<<"Failed subscribe to event"<<property->name();

//        if (property->name().compare(constInputCode)==0) {
//           QOpcUaMonitoredEvent *monitoredEvent=m_eventSubscription->addEvent(property);
//           if (monitoredEvent) {
//               qDebug()<<"Subscribe success to event"<<property->name();
//               QObject::connect(monitoredEvent, &QOpcUaMonitoredEvent::newEvent, this, &OpcUaClient::updateValue);
//               //return true;
//           }
//           else
//               qDebug()<<"Failed subscribe to event"<<property->name();
//        }
            qDebug()<<8;
            m_monitoredValues.append(m_valueSubscription->addValue(property));
            qDebug()<<9;
            if (m_monitoredValues.last()) {
//                m_monitoredValues.last()->setProperty(constObjName,property->property(constObjName));
                m_monitoredValues.last()->setProperty(constDevId,property->property(constDevId));
                m_monitoredValues.last()->setProperty(constPropName,property->name());
                qDebug()<<"Subscribe success to value"<<property->name();
                QObject::connect(m_monitoredValues.last(), &QOpcUaMonitoredValue::valueChanged,
                        this,&OpcUaClient::processValue);
//                        [this](QVariant value){
//                });
                return true;
            }
            else {
                m_monitoredValues.removeLast();
//                qDebug()<<"Can not subscribe to"<<*nodeName;
            }
        }
    qDebug()<<"Can not subscribe to"<<property->name();
    return false;
}

void OpcUaClient::processValue(QVariant value)
{
    QOpcUaMonitoredValue *sender = qobject_cast<QOpcUaMonitoredValue *>(QObject::sender());
//    qDebug()<<"valueChanged"<<value
//                            <<sender->property(constDevId).toInt()
//                            <<sender->property(constPropName).toString();
    if(sender) {
//        qDebug()<<"sender qobject_cast OK"<<sender->property(constDevId).toInt()
//                <<sender->property(constPropName).toString()<<value;
        emit propertyChanged(sender->property(constDevId).toInt(),
                             sender->property(constPropName).toString(),value);
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
