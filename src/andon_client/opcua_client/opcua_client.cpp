#include "opcua_client.h"

#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaMonitoredValue>
#include <QtOpcUa/QOpcUaMonitoredEvent>
#include <QtOpcUa/QOpcUaSubscription>
#include <QtOpcUa/QOpcUaNode>
#include <QtOpcUa/QOpcUaProvider>
#include <QtCore/QDebug>
#include <QtCore/QScopedPointer>


//#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
//#include <QApplication>

//#include <keep_alive.h>



//const QByteArray constObjName = "OBJECT_NAME";
const QByteArray constCodePropName = "PROPERTY_NAME";
const QByteArray constDevId = "TCPDEVICE_ID";
const QString constInputCode("inputCode");
const QByteArray constFullNodeName = "propertyName";

OpcUaClient::OpcUaClient(QObject *parent)
    : QObject(parent)
    , m_pClient(0)
    , m_pProvider(new QOpcUaProvider(this))
    , m_subscription(0)
    , m_monitorTimer(new QTimer)
    , m_pRoot(0)
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
            qDebug()<<2;
//            if (!m_pClient->isConnected()){
                bool res = m_pClient->connectToEndpoint(QString("opc.tcp://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT));
//                for(auto ts:m_pClient->findChildren<QTcpSocket*>()){
//                    qDebug()<<"tcpSocket found"<<ts;
//                    keep_alive(ts);
//                }
                qDebug()<<5;
                if (!res) {
                    qWarning() << QString("Failed to connect to endpoint: opc.tcp://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT);
                    exit(EXIT_FAILURE);
                }
                qDebug()<<6;
                if(m_pClient)
                    m_subscription = m_pClient->createSubscription(100);
                qDebug()<<7;
//                m_pRoot = m_pClient->node("ns=0;s=Objects");
    });

    QObject::connect(m_pClient, &QOpcUaClient::connectedChanged,[this](bool connected){
//        qDebug()<<"connected changed"<<connected;
        if (connected) {
            qDebug()<<3;
            m_isConnected=true;
            m_monitorTimer->start(0);
//            m_monitorTimer->setSingleShot(true);
            qDebug()<<4;
        }
        else
            qDebug()<<"connected changed"<<connected;
    });

    QObject::connect(m_monitorTimer, &QTimer::timeout,[this](){
                m_monitorTimer->setInterval(1800000);
                QMapIterator<int,QString> nodes(m_nodesMap);
                while (nodes.hasNext()) {
                    nodes.next();
                    qDebug()<<11;
                    monitorNode(nodes.key(),nodes.value());
                }
        });

    QTimer *connectTimer=new QTimer;
    QTcpSocket *testSocket=new QTcpSocket;
    QObject::connect(connectTimer,&QTimer::timeout,[=](){
//        qDebug()<<"OPCUA WATCHDOG search server";
        connectTimer->setInterval(OPCUA_INTERVAL);
            if (testSocket->bind(QHostAddress(OPCUA_IP),OPCUA_PORT)) {
                testSocket->close();
                if(m_isConnected){
                    m_pClient->disconnectFromEndpoint();
                    m_isConnected=false;
                    m_monitorTimer->stop();
                    m_subscription->deleteLater();
                    m_subscription=0;
//                    m_pRoot->deleteLater();
//                    m_pRoot=0;
                    qDebug()<<"OPCUA Server disconnected";
                }
//                qDebug()<<"OPCUA WATCHDOG testSocket->bind";
//                if (m_pClient->isConnected()){

//                    m_pClient->disconnectFromEndpoint();
//                    emit m_pClient->connectedChanged(false);
//                }
            }
            else {
                if (!m_isConnected){
//                    if (m_pClient->isConnected()) {
//                        qDebug()<<"OPCUA WATCHDOG m_pClient->isConnected()";
//                        m_pClient->disconnectFromEndpoint();
//                        while (!m_monitoredValues.isEmpty())
//                            m_monitoredValues.takeLast()->deleteLater();
//                        m_subscription->deleteLater();
//                        m_subscription=0;
//                    }
                    qDebug()<<"OPCUA WATCHDOG server found";
                    QTimer::singleShot(0,connectTimer,&QTimer::stop);
                    QTimer::singleShot(5000,this,//&OpcUaClient::serverFound);
                                       [=](){
                        qDebug()<<0;
                        if (!testSocket->bind(QHostAddress(OPCUA_IP),OPCUA_PORT))
                            emit serverFound();
                        connectTimer->start();
                        qDebug()<<8;
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
    delete m_subscription;
    delete m_pRoot;
    //delete m_eventSubscription;
    m_nodesMap.clear();
}



void OpcUaClient::monitorNode(int deviceId, const QString &objectName)
{
    qDebug()<<12;
//    if(m_pRoot && m_subscription){
//        qDebug()<<"m_pRoot"<<m_pRoot<<"m_subscription"<<m_subscription;
//        return;
//    }
    QString nodeId = QString(objectName).prepend("ns=2;s=");
//    if(!m_pRoot->childIds().contains(nodeId)){
//        qDebug()<<"!m_pRoot->childIds().contains(nodeId)";
//        return;
//    }
    QOpcUaNode* node = m_pClient->node(nodeId);
    if(node){

/*
                QOpcUaMonitoredEvent *monitoredEvent=m_subscription->addEvent(node);
                if (monitoredEvent) {
                    monitoredEvent->setObjectName("event_"+node->name().toLatin1());
                    monitoredEvent->setProperty("nodename",node->name());
                    node->setProperty("nodename",node->name());
                    monitoredEvent->setParent(node);
                    qDebug()<<"Subscribe success to event"<<node->name()
                            <<"monitoredEvent"<<monitoredEvent;
                    //D:\Docs\Qt projects\Andon_project\souces\lib\
                    //qtopcua\src\plugins\opcua\freeopcua\qfreeopcuavaluesubscription.cpp
                    //        val.push_back(QVariant(QString::fromStdString(event.Message.Text)));
                    //        val.push_back(QVariant(QString::fromStdString(event.SourceName)));
                    //        val.push_back(QVariant((double) event.Severity));
                    QObject::connect(monitoredEvent , &QOpcUaMonitoredEvent::newEvent,
                                     this,&OpcUaClient::processEvent);
//                    QObject::connect(monitoredEvent, &QOpcUaMonitoredEvent::newEvent,
//                                     [monitoredEvent,this](QVector<QVariant> value){
//                                qDebug() << monitoredEvent->objectName()
//                                         << monitoredEvent->property("nodename").toString() << value;
//                    });
                }
                else
                    qDebug()<<"Failed subscribe to event"<<node->name();
*/

        QStringList properties = node->childIds();
        qDebug()<<node<<node->name()<<properties.count()<<"properties";
            for(QString propertyName:node->childIds()){
                if(propertyName.split(".").last()==constInputCode){
                    QOpcUaNode* propertyNode = m_pClient->node(propertyName);
                    propertyNode->setProperty(constDevId,deviceId);
                    propertyNode->setProperty(constFullNodeName,propertyName);
                    subscribeProperty(propertyNode);
                }
            }
    }
    else
        qDebug()<<"Can not finde object"<<objectName;
}

bool OpcUaClient::subscribeProperty(QOpcUaNode *property)
{
    QString nodeName = property->property(constFullNodeName).toString();
    if (property) {
        QString monValName = QString("MonVal_").append(nodeName);
            QOpcUaMonitoredValue *monitoredValue =
                    m_subscription->findChild<QOpcUaMonitoredValue *>(monValName);
            if (monitoredValue){
                qDebug() << nodeName << "already subscribed. Value="<<monitoredValue->node().value();
                //TODO chack monitoredValue connection
                return true;
            }
            monitoredValue = m_subscription->addValue(property);
            if (monitoredValue) {
                qDebug()<<"Subscribe success to value"<<property<<monitoredValue;
/*   lambda emit propertyChanged
//                QObject::connect(monitoredValue, &QOpcUaMonitoredValue::valueChanged,
//                                 [this,monitoredValue](QVariant value){
//                    qDebug()<<"valueChanged"<<monitoredValue<<value;
//                    emit propertyChanged(monitoredValue->property(constDevId).toInt(),
//                                         monitoredValue->property(constCodePropName).toString(),value);
//                });
*/
                monitoredValue->setProperty(constDevId,property->property(constDevId));
                monitoredValue->setProperty(constCodePropName,property->name());
                QObject::connect(monitoredValue, &QOpcUaMonitoredValue::valueChanged,
                        this,&OpcUaClient::processValue);
                monitoredValue->setObjectName(monValName);
                monitoredValue->setParent(m_subscription);
//                if(m_subscription->findChild<QOpcUaMonitoredValue *>(monValName))
//                    qDebug()<< monValName <<"found in m_subscription";
//                else
//                    qDebug()<<QString("MonVal").append(nodeName)<<"not found in m_subscription";
                return true;
            }
        }
    qDebug()<<"Can not subscribe to"<<nodeName;
    return false;
}

void OpcUaClient::processValue(QVariant value)
{
    QOpcUaMonitoredValue *sender = qobject_cast<QOpcUaMonitoredValue *>(QObject::sender());
    if(sender) {
//        qDebug()<<"Device"<<sender->property(constDevId).toInt()
//                <<sender->property(constCodePropName).toString()<<value;
        emit propertyChanged(sender->property(constDevId).toInt(),
                             sender->property(constCodePropName).toString(),value);
    }

}

void OpcUaClient::processEvent(QVector<QVariant> value)
{
    qDebug() << 1;
    QOpcUaMonitoredEvent *sender = qobject_cast<QOpcUaMonitoredEvent *>(QObject::sender());
    if(sender) {
        qDebug() << sender->parent() << sender
                 << sender->property("nodename").toString()
                 << value;
    }
    if(sender) {
        qDebug() << sender->parent()->property("nodename").toString()
                 << sender->parent()->objectName() << sender->objectName();
    }

}

    void OpcUaClient::updateValue(QVector<QVariant> value)
{
    qDebug()<<value;
    //TODO feedback
}


void OpcUaClient::appendObject(int deviceId, const QString &objectName)
{
    qDebug()<<10;
    m_nodesMap.insert(deviceId,objectName);
    if (m_isConnected)
        monitorNode(deviceId,objectName);
}
