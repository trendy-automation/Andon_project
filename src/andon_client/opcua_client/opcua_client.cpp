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
    });

    QObject::connect(m_pClient, &QOpcUaClient::connectedChanged,[this](bool connected){
//        qDebug()<<"connected changed"<<connected;
        if (connected) {
            qDebug()<<3;
            m_isConnected=true;
            qDebug()<<4;
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
//        qDebug()<<"OPCUA search server";
        connectTimer->setInterval(OPCUA_INTERVAL);
            if (testSocket->bind(QHostAddress(OPCUA_IP),OPCUA_PORT)) {
                testSocket->close();
                m_isConnected=false;
            }
            else {
                if (!m_isConnected){
                    if (m_pClient->isConnected()) {
//                        qDebug()<<"m_monitorList m_valueSubscription deleteLater";
                        m_pClient->disconnectFromEndpoint();
                        while (!m_monitorList.isEmpty())
                            m_monitorList.takeLast()->deleteLater();
                        m_valueSubscription->deleteLater();
                    }
                    qDebug()<<"OPCUA server found";
                    emit serverFound();
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
        QStringList properties = node->childIds();
        //TODO all fields to constant string (inputCode)
        //TODO tag system in DB
        QListIterator<QString> prop(properties);
        qDebug()<<node->name()<<properties.count()<<"properties";
        while (prop.hasNext()) {
            QString propertiyName(prop.next());
            if(propertiyName.split(".").last()==constInputCode){
                QOpcUaNode* propertiyNode = m_pClient->node(propertiyName);
                propertiyNode->setProperty(constDevId,deviceId);
                subscribePropertiy(propertiyNode);
            }
            // else qDebug()<<propertiyName<<"not subscribe";
        }
    }
    else
        qDebug()<<"Can not finde object"<<objectName;
}

bool OpcUaClient::subscribePropertiy(QOpcUaNode *propertiy)
{
//    QString *nodeName= new QString(node->name());
    if (propertiy) {
//        if (propertiy->name().compare(constInputCode)==0) {
//           QOpcUaMonitoredEvent *MonitorEvt=m_eventSubscription->addEvent(propertiy);
//           if (MonitorEvt) {
//               qDebug()<<"Subscribe success to event"<<propertiy->name();
//               QObject::connect(MonitorEvt, &QOpcUaMonitoredEvent::newEvent, this, &OpcUaClient::updateValue);
//               return true;
//           }
//           else
//               qDebug()<<"Failed subscribe to event"<<propertiy->name();
//        }
            qDebug()<<8;
            m_monitorList.append(m_valueSubscription->addValue(propertiy));
            qDebug()<<9;
            if (m_monitorList.last()) {
//                m_monitorList.last()->setProperty(constObjName,propertiy->property(constObjName));
                m_monitorList.last()->setProperty(constDevId,propertiy->property(constDevId));
                m_monitorList.last()->setProperty(constPropName,propertiy->name());
                qDebug()<<"Subscribe success to value"<<propertiy->name();
                QObject::connect(m_monitorList.last(), &QOpcUaMonitoredValue::valueChanged,
                        this,&OpcUaClient::processValue);
//                        [this](QVariant value){
//                });
                return true;
            }
            else {
                m_monitorList.removeLast();
//                qDebug()<<"Can not subscribe to"<<*nodeName;
            }
        }
    qDebug()<<"Can not subscribe to"<<propertiy->name();
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
