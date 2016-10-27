#include <QtNetwork>
#include <QUdpSocket>
#include "bcsender.h"

#include <QByteArray>
#include <QDataStream>

//#include <QHostAddress>
//#include <QNetworkInterface>

//#define _STR(x) #x
//#define STR(X) _STR(x)


Sender::Sender(QObject* parent) :QObject(parent)
{
}

void Sender::run(int interval, int port)
{
//    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
//        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
//            qDebug() << address.toString();
//    }
    timer = new QTimer(this);
    timer->setInterval(interval);
    udpport=port;
//    qDebug() << interval<<udpport;
    udpSocket = new QUdpSocket(this);
    messageNo = 1;
    QObject::connect(timer, &QTimer::timeout, this, &Sender::broadcastDatagram);
    startBroadcasting();
}

void Sender::startBroadcasting()
{
//    qDebug() << "startBroadcasting";
    timer->start();
}

void Sender::stop()
{
    timer->stop();
    udpSocket->close();
    this->deleteLater();
}

void Sender::broadcastDatagram()
{
    QByteArray datagram = (UDP_SRVRNAME);//. msg num " + QByteArray::number(messageNo);
    for (int i=0;i<ClientList.count();++i)
        udpSocket->writeDatagram(datagram.data(), datagram.size(),
                             ClientList.at(i), udpport);
    //qDebug() << datagram<<udpport;

    //++messageNo;
    //TODO: if > max
}

void Sender::renewInterface(const QString &ClientIp)
{
    QByteArray datagram = "Andon client,renewInterface";
    if (ClientIp.isEmpty())
        for (int i=0;i<ClientList.count();++i)
            udpSocket->writeDatagram(datagram.data(), datagram.size(),
                                 ClientList.at(i), udpport);
    else if (!QHostAddress(ClientIp).isNull())
        udpSocket->writeDatagram(datagram.data(), datagram.size(),
                             QHostAddress(ClientIp), udpport);
    qDebug() << datagram << ClientIp;
}

void Sender::addClient(const  QString &IP)
{
    ClientList.append(QHostAddress(IP));
    qDebug() << "UDP Sender: Client added" << IP;
}

void Sender::clearClients()
{
    ClientList.clear();
}
