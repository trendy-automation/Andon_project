#ifndef OPCUA_CLIENT_H
#define OPCUA_CLIENT_H

#include <QVariant>
#include <QVector>
#include <QTimer>


QT_BEGIN_NAMESPACE
class QOpcUaClient;
class QOpcUaNode;
class QOpcUaProvider;
class QOpcUaMonitoredValue;
class QOpcUaMonitoredEvent;
class QOpcUaSubscription;
QT_END_NAMESPACE



class OpcUaClient : public QObject
{
    Q_OBJECT

public:
    explicit OpcUaClient(QObject *parent = 0);
    virtual ~OpcUaClient();

public slots:
    void monitorNode(int deviceId, const QString &objectName);
    void updateValue(QVector<QVariant> value);    
    void appendObject(int deviceId, const QString &objectName);


signals:
    void serverFound();
    void propertyChanged(int deviceId, const QString &variableName, const QVariant &value);

private slots:
    void processValue(QVariant value);
    void processEvent(QVector<QVariant> value);

private:
    bool subscribeProperty(int deviceId, const QString &variableName, QOpcUaNode *variable);
    void subscribeObjects();

//    QList<QOpcUaMonitoredValue*>  m_monitoredValues;
//    QList<QOpcUaMonitoredEvent*>  m_monitoredEvents;
    QOpcUaSubscription   *m_subscription;
//    QOpcUaSubscription   *m_eventSubscription;
    QOpcUaClient        *m_pClient;
    QOpcUaProvider      *m_pProvider;
    QMap<int,QString>    m_objectsMap;
    QTimer              *m_monitorTimer;
//    QOpcUaNode          *m_pRoot;
    bool                 m_isConnected=false;
};

#endif // OPCUA_CLIENT_H
