#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QHostAddress>

class QTimer;
class QUdpSocket;



class Sender : public QObject
{
Q_OBJECT
public:
    Sender(QObject *parent=0);
    void addClient(const  QString &IP);
    void clearClients();
    void run(int interval=UDP_INTERVAL,int port=UDP_PORT);
    void stop();
    void renewInterface(const QString &ClientIp="");

private slots:
    void startBroadcasting();
    void broadcastDatagram();

private:
    QUdpSocket *udpSocket;
    QTimer *timer;
    qint32 messageNo;
    int udpport;
    QList<QHostAddress> ClientList;
};

#endif
