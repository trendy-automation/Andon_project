#ifndef NODES_MANAGER_H
#define NODES_MANAGER_H

#include <opc/ua/node.h>
#include <opc/ua/subscription.h>
#include <opc/ua/server/server.h>
#include <opc/ua/event.h>
#include <opc/ua/protocol/node_management.h>
#include <opc/ua/protocol/string_utils.h>
#include <QVariantMap>
#include "ketcp_object.h"


namespace OpcUa
{
    class UaServer;
    class Event;
}

using namespace OpcUa;

class NodesManager : public QObject, public SubscriptionHandler
{
    Q_OBJECT
public:
    explicit NodesManager(QObject *parent = 0);
    virtual ~NodesManager();
    void initNodes();
    void loadKeObject(KeTcpObject *keObject);

    void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) Q_DECL_OVERRIDE;

public slots:
    //void adjustTemperature();
signals:
    //void propertyChanged(const Node& node, const Variant& val, AttributeId attr) Q_DECL_OVERRIDE;

private:
    Variant varConv(const QVariant& v);


//    double m_currentTemp;
//    double m_temperatureSetpoint;

    UaServer *m_server;
//    Node m_currentTempNode;
//    Node m_setPointNode;
//    Node m_startNode;
//    Node m_stopNode;
//    Node m_stateNode;
    Node m_root;
    uint32_t    m_idx;
//    std::unique_ptr<Subscription> sub ;
    std::unique_ptr<Subscription> m_subscription;
//    OpcUa::Event m_eventInjection;
    QMap<Node*,OpcUa::Event> m_events;

    //QTimer m_timer;
};

#endif // NODES_MANAGER_H
