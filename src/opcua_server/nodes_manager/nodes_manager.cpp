#include "nodes_manager.h"
#include <opc/ua/server/server.h>

#include <QVector>
#include <QDataStream>
#include <QBitArray>

#include <QTimer>

//#include <QApplication>


using namespace OpcUa;

//class SubClient : public SubscriptionHandler
//{
//    void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) override
//    {
//        qDebug() << handle << QString::fromStdString(node.ToString())
//                 << QString::fromStdString(val.ToString()) << (uint32_t)attr;
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
//    qDebug() << "" << 1;
    m_server->SetServerName("Andon OPCUA server");
//    qDebug() << "" << 2;
    m_server->SetEndpoint(QString("opc.tcp://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT).toStdString());
//    qDebug() << "" << 3;
    m_server->SetServerURI(QString("urn://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT).toStdString());
//    qDebug() << "" << 4;
    m_server->Start();
//    qDebug() << "" << 5;
    m_server->EnableEventNotification();
//    qDebug() << "" << 6;
    m_root = m_server->GetObjectsNode();
//    qDebug() << "" << 7;
    m_idx = m_server->RegisterNamespace("http://qt-project.org");
//    Node triggerVar = m_root.AddVariable("ns=3;s=TriggerVariable", "TriggerVariable", Variant(0));
//    Node triggerNode = m_root.AddObject("ns=3;s=TriggerNode", "TriggerNode");

//    m_eventInjection = OpcUa::Event(ObjectId::BaseEventType);
//    m_eventInjection.Severity = 2;
//    m_eventInjection.SourceName = "Injection_done";
//    m_eventInjection.SourceNode=triggerNode.GetId();
//    m_eventInjection.Time = DateTime::Current();
//    m_eventInjection.Message = LocalizedText("Injection event");
//    m_server->TriggerEvent(m_eventInjection);

    qDebug() << "" << 8;
//    // Workaround for not having server side methods
//    SubClient clt;
//    clt.m_event = &m_eventInjection;
//    clt.m_server = m_server;
//    qDebug() << "" << 9;
//    std::unique_ptr<Subscription> sub = m_server->CreateSubscription(100, *this);
//    qDebug() << "" << 10;
//    sub->SubscribeDataChange(triggerVar);
//    qDebug() << "" << 11;
}

NodesManager::~NodesManager()
{
    m_server->Stop();
}


//NodesManager::~NodesManager()
//{
//}

//void NodesManager::initNodes()
//{

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
//}

//void NodesManager::DataChange(uint32_t handle, const Node &node, const Variant &val, AttributeId attr)
//{
//    Q_UNUSED(handle);
//    Q_UNUSED(attr);

//    qDebug() << handle << QString::fromStdString(node.ToString()) << QString::fromStdString(val.ToString()) << (uint32_t)attr;

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
//}

void NodesManager::loadKeObject(KeTcpObject *keObject)
{

    QString keName = keObject->getDeviceName();
    qDebug()<<"AddObject"<<QString("ns=2;s=%1").arg(keName);
    Node *keNode = new Node(m_root.AddObject(QString("ns=2;s=%1").arg(keName).toStdString(),
                             keName.toStdString()));
    QTimer *keWatchTimer = new QTimer;
    QObject::connect(keWatchTimer,&QTimer::timeout,[keNode](){
        qDebug()<<keNode<<keNode->GetChildren().size();
    });
    keWatchTimer->start(1800000);
//    qDebug()<<"m_events.insert(keNode,new OpcUa::Event(keNode))";
//    OpcUa::Event *evt = new OpcUa::Event(ObjectId::BaseEventType);
//    if(evt){
//        evt->Severity = 2;
//        evt->SourceNode = keNode->GetId();
//        evt->SourceName = keNode->GetId().GetStringIdentifier();
//        evt->Time = DateTime::Current();
//        m_events.insert(keNode,evt);
//    }

            QObject::connect(keObject,&KeTcpObject::disconnected,[this,keObject,keNode,keName](){
                qDebug() << keName << "disconnected";
//                m_root.GetChild(keName.toStdString()).SetValue(Variant());
//                keNode->SetValue(Variant());
//                delete keNode;
                //TODO: clean Object if it is disconnected
                //keObject.SetValue(objects.AddFolder(name.prepend("ns=3;s=").toStdString(), name.toStdString()));
            });


            QObject::connect(keObject,&KeTcpObject::ready,[this,keObject,keNode,keName](){ //this,&NodesManager::readProperties);
                QVariantMap properties = keObject->getProperties();
                qDebug() << &keName << "ready" << properties.count();
               for (auto p=properties.constBegin();p!=properties.constEnd();++p){
                    for (Node &v:keNode->GetChildren())
                        if(v.GetBrowseName().Name==p.key().toStdString()) {
                            v.SetValue(varConv(p.value()));
                            return;
                        }
                    Node * propertyNode = new Node(keNode->AddProperty(
                        QString("ns=3;s=%1.%2").arg(keName).arg(p.key()).toStdString(),
                        p.key().toStdString(), varConv(p.value())));
                    qDebug()<< "Add property" << p.key().toLatin1() << "=" << p.value() << propertyNode;
                }
             });


            QObject::connect(keObject,&KeTcpObject::IOEvent,
                             [this,keObject,keNode](const QString &ioName,const QVariant &val){
//                qDebug()<< keObject->getDeviceName() << ioName << val;

//                if(m_events.contains(keNode) && keNode){
//                    OpcUa::Event *evt=m_events.value(keNode);
//                    evt->SetValue(keNode->GetBrowseName(),varConv(val));
//                    evt->Message = LocalizedText(QString("keNode event %1 value")
//                        .arg(val.value<quint32>()).toStdString());
//                    evt->SourceName=ioName.toStdString();
//                    evt->Time=DateTime::Current();
//                    qDebug() << QString("keNode event %1 %2")
//                               .arg(val.value<quint32>()).arg(ioName);
//                    m_server->TriggerEvent(*evt);
//                }
//               std::vector<Node> nodeVec=keNode->GetChildren();
                for (Node &v:keNode->GetChildren())
                    if(v.GetBrowseName().Name==ioName.toStdString()) {
                        qDebug()<< keObject->getDeviceName() << ioName << val << keNode << &v;
                        v.SetValue(varConv(val));
                        return;
                    }
//                    else
                qDebug()<< keObject->getDeviceName() << "missing child" << ioName;
            });
//            QTimer::singleShot(15000,[keObject](){
//                keObject->IOEvent("inputCode",32);
//            });
}
void NodesManager::readProperties()
{

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
