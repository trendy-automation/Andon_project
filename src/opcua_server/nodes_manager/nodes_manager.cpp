#include "nodes_manager.h"
#include <opc/ua/server/server.h>

#include <QVector>
#include <QDataStream>
#include <QBitArray>

//#include <QApplication>


using namespace OpcUa;

//class SubClient : public SubscriptionHandler
//{
//    void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) override
//    {
//        Q_UNUSED(handle);
//        qDebug() << handle << QString::fromStdString(node.ToString()) << QString::fromStdString(val.ToString()) << (uint32_t)attr;
//        m_server->TriggerEvent(*m_event);
//    }
//public:
//    UaServer *m_server;
//    OpcUa::Event *m_event;
//};


NodesManager::NodesManager(QObject *parent)
    : QObject(parent), m_server(new UaServer(false /*debug*/))
{
//    QObject::connect(QApplication::instance(), &QApplication::aboutToQuit, [this](){
//        qDebug("Stopping server");
//        m_server->Stop();
//    });
//    m_server = new UaServer(false /*debug*/);
//    qDebug() << 1;
    m_server->SetServerName("Andon OPCUA server");
//    qDebug() << 2;
    m_server->SetEndpoint(QString("opc.tcp://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT).toStdString());
//    qDebug() << 3;
    m_server->SetServerURI(QString("urn://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT).toStdString());
//    qDebug() << 4;
    m_server->Start();
//    qDebug() << 5;
    m_server->EnableEventNotification();
//    qDebug() << 6;
    m_root = m_server->GetObjectsNode();
//    qDebug() << 7;
    m_idx = m_server->RegisterNamespace("http://qt-project.org");
//    m_eventInjection = OpcUa::Event(ObjectId::BaseEventType);

//    // Workaround for not having server side methods
//    SubClient clt;
//    clt.m_event = &m_eventInjection;
//    clt.m_server = m_server;
//    std::unique_ptr<Subscription> sub = m_server->CreateSubscription(100, *this);
//    sub->SubscribeDataChange(m_root);
}

NodesManager::~NodesManager()
{
    m_server->Stop();
}


//NodesManager::~NodesManager()
//{
//}

void NodesManager::initNodes()
{

//    m_subscription = server->CreateSubscription(100, *this);

//    m_eventInjection = new Event(ObjectId::BaseEventType); //you should create your own type
//    m_eventInjection.EventId;


//    OpcUa::Event ev(ObjectId::BaseEventType);
//    m_eventInjection = ev;
//    m_eventInjection = OpcUa::Event(ObjectId::BaseEventType);
//    m_eventInjection.SetValue("Injection_done",56);
//    m_eventInjection.Severity = 2;
//    m_eventInjection.SourceNode = m_root.GetId(); //propertyNode.GetId();
//    m_eventInjection.SourceName = "Injection_done";
//    m_eventInjection.Time = DateTime::Current();
//    m_eventInjection.Message = LocalizedText("Part_injected");
//    m_server->TriggerEvent(m_eventInjection);

//    Node acControl = m_root.AddFolder("ns=3;s=ACControl", "ACControl");

//    m_setPointNode = acControl.AddVariable("ns=3;s=ACControl.SetPoint", "ACSetpoint", Variant(1));
//    m_currentTempNode = acControl.AddVariable("ns=3;s=ACControl.CurrentTemp", "ACCurrentTemp", Variant(2));

//    m_startNode = acControl.AddVariable("ns=3;s=ACControl.Start", "ACStart", Variant(true));
//    m_stopNode = acControl.AddVariable("ns=3;s=ACControl.Stop", "ACStop", Variant(false));
//    m_stateNode = acControl.AddVariable("ns=3;s=ACControl.IsRunning", "ACIsRunning", Variant(true));

//    // get set point updates
//    m_subscription = server->CreateSubscription(100, *this);
//    m_subscription->SubscribeDataChange(m_setPointNode);

//    // this is a workaround for method calls (not yet implemented in FreeOpcUa)
//    m_subscription->SubscribeDataChange(m_startNode);
//    m_subscription->SubscribeDataChange(m_stopNode);
}

void NodesManager::DataChange(uint32_t handle, const Node &node, const Variant &val, AttributeId attr)
{
//    Q_UNUSED(handle);
//    Q_UNUSED(attr);

    qDebug() << handle << QString::fromStdString(node.ToString()) << QString::fromStdString(val.ToString()) << (uint32_t)attr;

//    if (node == m_setPointNode)
//        m_temperatureSetpoint = val.As<double>();

//    if (node == m_startNode && val.As<bool>()) {
//        m_timer.metaObject()->invokeMethod(&m_timer, "start");
//        m_stateNode.SetValue(Variant(true));
//    }
//    if (node == m_stopNode && val.As<bool>()) {
//        m_timer.metaObject()->invokeMethod(&m_timer, "stop");
//        m_stateNode.SetValue(Variant(false));
//    }
}

void NodesManager::loadKeObject(KeTcpObject *keObject)
{
            Node * keNode = new Node(m_root.AddObject(
                                     QString("ns=2;s=%1").arg(keObject->getDeviceName()).toStdString(),
                                     keObject->getDeviceName().toStdString()));
//            qDebug()<<"AddObject"<<QString("ns=2;s=%1").arg(keObject->getDeviceName());
            QObject::connect(keObject,&KeTcpObject::disconnected,[this,keObject,keNode](){
                qDebug() << keObject->getDeviceName() << "disconnected";
//                m_root.GetChild(keObject->getDeviceName().toStdString()).SetValue(Variant());
//                keNode->SetValue(Variant());
//                delete keNode;
                //TODO: clean Object if it is disconnected
                //keObject.SetValue(objects.AddFolder(name.prepend("ns=3;s=").toStdString(), name.toStdString()));
            });

//            QMetaObject::Connection m_connection =
                    QObject::connect(keObject,&KeTcpObject::ready,[this,keObject,keNode](){ //&m_connection
                QVariantMap properties = keObject->getProperties();
                qDebug() << keObject->getDeviceName() << "ready" << properties.count();
                std::vector<Node> evtNodes = keNode->GetChildren();
                for (int i=0; i<properties.count(); ++i){
                    bool isExist=false;
                    for (int j=0;j<evtNodes.size();++j)
                        if(evtNodes.at(j).GetBrowseName().Name==properties.keys().at(i).toStdString()) {
                            isExist=true;
                            evtNodes.at(j).SetValue(varConv(properties.values().at(i)));
                            break;
                        }
//                    qDebug() << keObject->getDeviceName() << "ready";
                    if (!isExist) {
                        qDebug() << properties.keys().at(i).toLatin1() << "=" << properties.values().at(i);
                        Node propertyNode(keNode->AddProperty(
                            QString("ns=3;s=%1.%2").arg(keObject->getDeviceName()).arg(properties.keys().at(i)).toStdString(),
                            properties.keys().at(i).toStdString(), varConv(properties.values().at(i))));


//                        if(properties.keys().at(i).toLatin1()=="inputCode") {
//                            m_eventInjection.SourceNode = propertyNode.GetId();
//                            //m_subscription->SubscribeDataChange(propertyNode.GetId());
//                        }


    //                    m_subscription->SubscribeDataChange(propertyNode);
                    }
                }
//                QObject::disconnect(m_connection);
            });



            QObject::connect(keObject,&KeTcpObject::IOEvent,[this,keObject,keNode](const QString &ioName,const QVariant &val){
                qDebug()<< keObject->getDeviceName() << ioName << val;
              std::vector<Node> evtNodes = keNode->GetChildren();
                //TODO save IDs for properties
//                Node evtNode=keNode->GetChild(ioName.toStdString());
              bool isExist = false;
              for (int i=0;i<evtNodes.size();++i)
                  if(evtNodes.at(i).GetBrowseName().Name==ioName.toStdString()) {
//                      if(evtNodes.at(i).GetValue()!=varConv(val))
                          evtNodes.at(i).SetValue(varConv(val));
                      isExist = true;
                  }
              if (!isExist)
                  qDebug()<< keObject->getDeviceName() << "missing child" << ioName;
//              else
//                  if(ioName=="inputCode"){
// //                      qDebug()  << keObject->getDeviceName() << "event inputCode" << val.value<quint32>();
//                      m_eventInjection.Severity = 2;
//                      m_eventInjection.SourceNode = keNode->GetId(); //propertyNode.GetId();
//                      m_eventInjection.SourceName = "Injection_done";
//                      m_eventInjection.Time = DateTime::Current();
//                      m_eventInjection.Message = LocalizedText(QString("Part with code %1 is injected")
//                          .arg(val.value<quint32>()).toStdString());
// //                      m_eventInjection.SetValue(ioName.toStdString(),varConv(val));
//                      m_server->TriggerEvent(m_eventInjection);
//                  }
            });
}

Variant NodesManager::varConv(const QVariant& v) {
//    qDebug()<< QString("Casting to Variant from QMetaType::%1. TypeId %2.")
//.arg(v.typeName()).arg((int)v.type());
    switch(v.userType()) {
        case QVariant::BitArray:
            {
            QBitArray ba = v.value<QBitArray>();
            std::vector<bool> vec(ba.size());
            for(int b=0; b<ba.count(); ++b)
                vec[b]=ba.testBit(b);
            return Variant( boost::any(vec));
        }
        case QVariant::Bool:
            return Variant( boost::any(v.value<bool>()));
        case QVariant::ByteArray:
            return Variant( boost::any(v.value<QByteArray>().toStdString()));
        case QVariant::Char:
            return Variant( boost::any(v.value<QChar>()));
        case QVariant::String:
            return Variant( boost::any(v.value<QString>().toStdString()));
        case QVariant::StringList:
            return Variant( boost::any(v.value<QStringList>().toStdList()));
        case QVariant::Time:
            return Variant( boost::any(v.value<QTime>()));
        case QVariant::Date:
            return Variant( boost::any(v.value<QDate>()));
        case QVariant::DateTime:
            return Variant( boost::any(v.value<QDateTime>()));
        case QVariant::Double:
            return Variant( boost::any(v.value<double>()));
        case QMetaType::Float:
            return Variant( boost::any(v.value<float>()));
        case QVariant::LongLong:
            return Variant( boost::any(v.value<qlonglong>()));
        case QVariant::Int:
            return Variant( boost::any(v.value<int>()));
        case QVariant::UInt:
            return Variant( boost::any(v.value<unsigned>()));
        case QMetaType::UShort:
            return Variant( boost::any(v.value<ushort>()));
        case QVariant::ULongLong:
            return Variant( boost::any(v.value<qulonglong>()));
        case QVariant::UserType:
        case QVariant::Invalid:
        default:
            qDebug()<< QString("Bad casting of type %1. TypeId %2.").arg(v.typeName()).arg((int)v.type());
            throw std::bad_cast(); //or stop App
            return boost::any();     //and return default constructed boost::any
    }
}
