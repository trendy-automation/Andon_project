#include "nodes_manager.h"
//#include <opc/ua/server/server.h>
//#include <opc/ua/node.h>
//#include <opc/ua/subscription.h>
//#include <opc/ua/event.h>


#include <QVector>
#include <QDataStream>
#include <QBitArray>

#include <QTimer>

//#include <QApplication>


using namespace OpcUa;

/*class SubClient : public SubscriptionHandler
{
    void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) override
    {
        qDebug() << handle << QString::fromStdString(node.ToString())
                 << QString::fromStdString(val.ToString()) << (uint32_t)attr;
        if(m_server && m_event)
            m_server.TriggerEvent(*m_event);
    }
public:
    UaServer m_server;
    OpcUa::Event *m_event;
};*/


NodesManager::NodesManager(QObject *parent)
    : QObject(parent)//, m_server(new UaServer(false /*debug*/))
{
//    m_server = new UaServer(false /*debug*/);
    m_server.SetServerName("Andon OPCUA server");
    m_server.SetEndpoint(QString("opc.tcp://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT).toStdString());
    m_server.SetServerURI(QString("urn://%1:%2").arg(OPCUA_IP).arg(OPCUA_PORT).toStdString());
    m_server.Start();
    m_server.EnableEventNotification();
    m_root = m_server.GetObjectsNode();
    m_idx = m_server.RegisterNamespace("http://10.208.110.75:8081/");
    m_triggerNode = m_server.GetRootNode().AddObject(QString("ns=1;s=TriggerNode").toStdString(),
                                              QString("TriggerNode").toStdString());
        m_eventObjects = OpcUa::Event(ObjectId::BaseEventType);
        m_eventObjects.Severity = 1;
        m_eventObjects.SourceName = "Object_event";
        m_eventObjects.SourceNode=m_root.GetId();
        m_eventObjects.Time = DateTime::Current();
        m_eventObjects.Message = LocalizedText("Object event");

/*//    Node triggerVar = m_root.AddVariable("ns=3;s=TriggerVariable", "TriggerVariable", Variant(0));
//    Node triggerNode = m_root.AddObject("ns=3;s=TriggerNode", "TriggerNode");
//    qDebug() << "m_root.GetId().GetNamespaceIndex" << m_root.GetId().GetNamespaceIndex();// QString::fromStdString();
////    qDebug() << "m_root.GetId().GetEncodingValue" << m_root.GetId().GetEncodingValue();// QString::fromStdString();
////    qDebug() << "m_root.GetId().GetIntegerIdentifier" << m_root.GetId().GetIntegerIdentifier();
////    qDebug() << "m_root.GetId().GetBinaryIdentifier" << m_root.GetId().GetBinaryIdentifier();
////    m_root.SetAttribute(AttributeId::BrowseName,DataValue(QualifiedName("Objects")));
//    m_root.SetAttribute(AttributeId::NodeId,DataValue(NodeId("Objects",0)));
//    qDebug() << "m_root.GetBrowseName()" << QString::fromStdString(m_root.GetBrowseName().Name);
//    m_eventInputCode = OpcUa::Event(ObjectId::BaseEventType);
//    m_eventInputCode.Severity = 2;
//    m_eventInputCode.SourceName = "Injection_done";
//    //m_eventInputCode.SourceNode=triggerNode.GetId();
//    m_eventInputCode.Time = DateTime::Current();
//    m_eventInputCode.Message = LocalizedText("Injection event");
//    QTimer::singleShot(15000,[this](){
//        m_server.TriggerEvent(m_eventInputCode);
//    });


//    qDebug() << "" << 8;
//    // Workaround for not having server side methods
//    SubClient clt;
//    qDebug() << "" << 8;
//    clt.m_event = &m_eventInputCode;
//    qDebug() << "" << 8;
//    clt.m_server = m_server;
//    qDebug() << "" << 9;*///

    sub = m_server.CreateSubscription(100, *this);
    qDebug() << "Subscription" << sub.get();
}

NodesManager::~NodesManager()
{
    m_server.Stop();
}

void NodesManager::DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr)
{
    qDebug() //<< QString::fromStdString(node.GetBrowseName().Name)
             << QString::fromStdString(node.GetId().GetStringIdentifier()) << &node << QString::fromStdString(val.ToString());
//             << ((val.Type()==VariantType::NUL)?(QVariant()):(
//                       (val.Type()==VariantType::UINT32)?QVariant(val.As<uint32_t>()):QVariant("unknown type")));
//    qDebug() << handle << QString::fromStdString(node.ToString())
//             << QString::fromStdString(node.GetId().GetStringIdentifier())
//             << QString::fromStdString(val.ToString()) << (uint32_t)attr;
}

void NodesManager::loadKeObject(KeTcpObject *keObject)
{

    QString keName = keObject->getDeviceName();
    qDebug()<<"AddObject"<<QString("ns=2;s=%1").arg(keName);
    m_objects.insert(keObject, m_root.AddObject(QString("ns=2;s=%1").arg(keName).toStdString(),
                                   keName.toStdString()));
    Node varNode = m_objects.value(keObject).AddVariable(
        QString("ns=3;s=%1.%2").arg(keObject->getDeviceName()).arg("loaded").toStdString(),
        QString("loaded").toStdString(), varConv(true));
    sub->SubscribeDataChange(varNode);
    m_server.TriggerEvent(m_eventObjects);

    QTimer *keWatchTimer = new QTimer;
    QObject::connect(keWatchTimer,&QTimer::timeout,
                     [this,keObject](){
        //qDebug()<<keName;
        refreshNodes(keObject);
    });
    keWatchTimer->start(300000);
/*
    qDebug()<<"m_events.insert(keNode,new OpcUa::Event(keNode))";
    OpcUa::Event *evt = new OpcUa::Event(ObjectId::BaseEventType);
    if(evt){
        evt->Severity = 2;
        evt->SourceNode = keNode.GetId();
        evt->SourceName = keNode.GetId().GetStringIdentifier();
        evt->Time = DateTime::Current();
        m_events.insert(keNode,evt);
    }*///

            QObject::connect(keObject,&KeTcpObject::disconnected,[this,keObject](){
                qDebug() << keObject->getDeviceName() << "disconnected";
                Node keNode = m_objects.value(keObject);
                for (Node &v:keNode.GetChildren()){
                    v.SetValue(Variant());
                }
                m_server.TriggerEvent(m_eventObjects);
            });

            QObject::connect(keObject,&KeTcpObject::ready,[this,keObject](){
                appendVariables(keObject);
/*                QMapIterator<QString,QVariant> p(properties);
                //for (auto p=properties.constBegin();p!=properties.constEnd();++p){
                while (p.hasNext()) {
                    p.next();
                    qDebug() << 1;
                    for (Node &v:keNode.GetChildren())
                        if(v.GetBrowseName().Name==p.key().toStdString()) {
                            qDebug() << 2;
                            v.SetValue(varConv(p.value()));
                            return;
                        }
                    Node varNode = keNode.AddVariable(
                        QString("ns=3;s=%1.%2").arg(keName).arg(p.key()).toStdString(),
                        p.key().toStdString(), varConv(p.value()));
                    qDebug()<< "Add variable" << p.key().toLatin1() << "=" << p.value() << &varNode;
                    sub->SubscribeDataChange(varNode);
                }*///
             });


            QObject::connect(keObject,&KeTcpObject::IOEvent,
                             [this,keObject](const QString &ioName,const QVariant &val){
                processEvent(keObject,ioName,val);
/*                qDebug()<< keObject->getDeviceName() << ioName << val;
                if(m_events.contains(keNode) && keNode){
                    OpcUa::Event *evt=m_events.value(keNode);
                    evt->SetValue(keNode.GetBrowseName(),varConv(val));
                    evt->Message = LocalizedText(QString("keNode event %1 value")
                        .arg(val.value<quint32>()).toStdString());
                    evt->SourceName=ioName.toStdString();
                    evt->Time=DateTime::Current();
                    qDebug() << QString("keNode event %1 %2")
                               .arg(val.value<quint32>()).arg(ioName);
                    m_server.TriggerEvent(*evt);
                }*///
/*                for (Node &v:keNode.GetChildren())
                    if(v.GetBrowseName().Name==ioName.toStdString()) {
                        Variant value=varConv(val);
                        qDebug()<< keObject->getDeviceName() << ioName << val  << &v;
                        v.SetValue(value);
                        m_eventInputCode = OpcUa::Event(ObjectId::BaseEventType);
                        m_eventInputCode.Severity = 15;
                        m_eventInputCode.SourceName = keObject->getDeviceName().toStdString();
                        m_eventInputCode.Time = DateTime::Current();
                        m_eventInputCode.Message = LocalizedText(ioName.toStdString());
                        m_eventInputCode.SetValue(v.GetBrowseName(),value);
                        m_eventInputCode.LocalTime = DateTime::Current();
                        m_eventInputCode.ReceiveTime = DateTime::Current();
                        m_eventInputCode.SourceNode=v.GetId();
                        m_server.TriggerEvent(m_eventInputCode);
                        return;
                    }
                qDebug()<< keObject->getDeviceName() << "missing child" << ioName;*///
            });
}

void NodesManager::appendVariables(KeTcpObject *keObject)
{
    QVariantMap variables = keObject->getProperties(QStringList()<<"inputCode");
//    qDebug() << keObject->getDeviceName() << variables.count();
    Node keNode = m_objects.value(keObject);
    for (Node &v:keNode.GetChildren()) {
        QString variableName = QString::fromStdString(v.GetBrowseName().Name);
        if(variables.contains(variableName)) {
            qDebug() << QString::fromStdString(v.GetId().GetStringIdentifier())
                     << "SetValue" << variables.value(variableName);
            v.SetValue(varConv(variables.take(variableName)));
        }
    }
    if(!variables.isEmpty()){
        QMapIterator<QString,QVariant> v(variables);
        while (v.hasNext()) {
            v.next();
            Node varNode = keNode.AddVariable(
                QString("ns=3;s=%1.%2").arg(keObject->getDeviceName()).arg(v.key()).toStdString(),
                v.key().toStdString(), varConv(v.value()));
            qDebug() << keObject->getDeviceName() << "Add variable" << v.key().toLatin1() << "=" << v.value() << &varNode;
            sub->SubscribeDataChange(varNode);
            qDebug() << "Subscription" << sub.get();
        }
        m_server.TriggerEvent(m_eventObjects);
    }
}

void NodesManager::processEvent(KeTcpObject *keObject, const QString &ioName, const QVariant &val)
{
    Node keNode = m_objects.value(keObject);
    for (Node &v:keNode.GetChildren())
        if(v.GetBrowseName().Name==ioName.toStdString()) {
            Variant value=varConv(val);
            qDebug()<< keObject->getDeviceName() << ioName << val << &v;
            v.SetValue(value);
//            m_eventInputCode = OpcUa::Event(ObjectId::BaseEventType);
//            m_eventInputCode.Severity = 1;
//            m_eventInputCode.SourceName = keObject->getDeviceName().toStdString();
//            m_eventInputCode.Time = DateTime::Current();
//            m_eventInputCode.Message = LocalizedText(ioName.toStdString());
//            m_eventInputCode.SetValue(v.GetBrowseName(),value);
//            m_eventInputCode.LocalTime = DateTime::Current();
//            m_eventInputCode.ReceiveTime = DateTime::Current();
//            m_eventInputCode.SourceNode=v.GetId();
//            m_server.TriggerEvent(m_eventInputCode);
            return;
        }
    qDebug()<< keObject->getDeviceName() << "missing child" << ioName;
}

void NodesManager::refreshNodes(KeTcpObject *keObject)
{
    Node keNode = m_objects.value(keObject);
//    qDebug()<< "Start refresh"<<QString::fromStdString(keNode.GetBrowseName().Name);
    for (auto &v:keNode.GetChildren()){
        Variant val = v.GetValue();
        if(val.Type()==VariantType::UINT32)
            if(val.As<uint32_t>()!=0)
                continue;
        qDebug() << "var"<<QString::fromStdString(v.GetId().GetStringIdentifier())
                 << QString::fromStdString(val.ToString());
//                << ((val.Type()==VariantType::NUL)?(QVariant()):(QVariant(val.As<uint32_t>())));
        v.SetValue(val);
    }

//    std::vector<Node> nodes = keNode.GetChildren();
//    for (std::vector<Node>::iterator it = nodes.begin(); it != nodes.end(); ++it){
//        qDebug()<< "var"<<QString::fromStdString(*it.GetBrowseName().Name)<<it;
//        Variant value = *it.GetValue();
//        if (value==Variant(0))
//            *it.SetValue(value);
//    }
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
