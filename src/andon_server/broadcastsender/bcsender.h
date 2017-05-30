#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QHostAddress>

class QTimer;
class QUdpSocket;



class BCSender : public QObject
{
Q_OBJECT
public:
    BCSender(int interval=UDP_INTERVAL,int port=UDP_PORT,QObject *parent=0);
    void addClient(const  QString &IP);
    void clearClients();
    void run();
    void stop();
    void renewInterface(const QString &ClientIp="");

public slots:
    void addClients(const QString &jsonClients);

private slots:
    void startBroadcasting();
    void broadcastDatagram();

private:
    int bcInterval;
    int bcPort;
    QUdpSocket *udpSocket;
    QTimer *timer;
    qint32 messageNo;
    int udpport;
    QList<QHostAddress> ClientList;
};

#endif
