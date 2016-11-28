#ifndef NODES_MANAGER_H
#define NODES_MANAGER_H

#include <opc/ua/node.h>
#include <opc/ua/subscription.h>
#include <opc/ua/server/server.h>
#include <opc/ua/event.h>
//#include <opc/ua/protocol/node_management.h>
//#include <opc/ua/protocol/string_utils.h>
#include <QVariantMap>
#include "ketcp_object.h"


//namespace OpcUa
//{
//    class UaServer;
//    class Event;
//}

QT_BEGIN_NAMESPACE
class UaServer;
class Event;
//class OpcUa;
QT_END_NAMESPACE

using namespace OpcUa;

class NodesManager : public QObject, public SubscriptionHandler
{
    Q_OBJECT
public:
    explicit NodesManager(QObject *parent = 0);
    virtual ~NodesManager();
    void loadKeObject(KeTcpObject *keObject);
    void DataChange(uint32_t handle, const OpcUa::Node &node, const OpcUa::Variant &val, OpcUa::AttributeId attr);
signals:
    //void propertyChanged(const Node& node, const Variant& val, AttributeId attr) Q_DECL_OVERRIDE;

private slots:
    void appendVariables(KeTcpObject *keObject, Node *keNode);
    void processEvent(KeTcpObject *keObject, Node *keNode, const QString &ioName,const QVariant &val);
    void refreshNodes(Node *keNode);

private:
    Variant varConv(const QVariant& v);


//    double m_currentTemp;
//    double m_temperatureSetpoint;

    OpcUa::UaServer m_server;
//    Node m_currentTempNode;
//    Node m_setPointNode;
//    Node m_startNode;
//    Node m_stopNode;
//    Node m_stateNode;
    Node m_root;
    uint32_t    m_idx;
    std::unique_ptr<Subscription> sub ;
//    std::unique_ptr<Subscription> m_subscription;
    OpcUa::Event m_eventInputCode;
    OpcUa::Event m_eventObjects;
//    QMap<Node*,OpcUa::Event*> m_events;

    //QTimer m_timer;
};

#endif // NODES_MANAGER_H
