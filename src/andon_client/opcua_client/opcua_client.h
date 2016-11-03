#ifndef OPCUA_CLIENT_H
#define OPCUA_CLIENT_H

#include <QVariant>
#include <QVector>


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
    void processValue(QVariant value);
    void appendObject(int deviceId, const QString &objectName);


signals:
    void serverFound();
    void propertyChanged(int deviceId, const QString &propertyName, const QVariant &value);
//    void newEvent(QVector<QVariant> value) const;

private:
    bool subscribePropertiy(QOpcUaNode *propertiy);

    QList<QOpcUaMonitoredValue*>  m_monitorList;
    QOpcUaSubscription   *m_valueSubscription;
    QOpcUaSubscription   *m_eventSubscription;

    QOpcUaClient        *m_pClient;
    QOpcUaProvider      *m_pProvider;
    QMap<int,QString>    m_nodesMap;
    bool                 m_isConnected=false;
};

#endif // OPCUA_CLIENT_H
