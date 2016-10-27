#ifndef RECEIVER_H
#define RECEIVER_H


#include <QObject>
#include <QHostAddress>
//#include <QString>

class QUdpSocket;



class UdpReceiver : public QObject
{
Q_OBJECT
public:
    UdpReceiver();
    void start();
    void stop();


signals:
    void serverfound(QHostAddress ServerAddress);
    void renewInterface(QHostAddress ServerAddress);

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *udpSocket;
    bool isserverfound;
    QHostAddress SvrHost;
};

#endif
