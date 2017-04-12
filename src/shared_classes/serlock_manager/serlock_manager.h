//*******************************************************************************

#ifndef SHERLOCK_MANAGER_H
#define SHERLOCK_MANAGER_H

#include <QObject>
#include <QTcpSocket>
//#include <QTcpServer>
#include <QStateMachine>
#include <QFinalState>
#include <QTimer>
#include <QHostAddress>
//#include <functional>



//*******************************************************************************
class SherlockManager: public QObject
{
    Q_OBJECT


public:
    SherlockManager(const QString &tcpServerIp="", quint16 tcpServerPort=0,
                    QObject * parent=0, bool keepConnect=true, bool connectNow=true);
    ~SherlockManager();
    void setClientIp(const QString &tcpServerIp){if (!isSocketConnected()) {serverIp=tcpServerIp; startConnecting();}}
    void setPort(quint16 tcpServerPort){if (!isSocketConnected()) {serverPort=tcpServerPort; startConnecting();}}
    bool isSocketConnected() {return tcpSocket->state() == QAbstractSocket::ConnectedState;}
    void setAutoReconnect(bool connectNow=true){autoReconnect=connectNow;}
    void setAutoConnect(bool keepConnect=true){autoConnect=keepConnect;}
    QVariantMap getProperties(const QStringList &requested);
    void setProperties(const QVariantMap &smProperties);

public slots:
    void doReceive(const QString &response);
    void Send(QString Data);
    void startConnecting();
    void VisuonRun();
    void VisionStop();

signals:
    void connectionStateChanged(bool isConnected);
    void lineReceived(const QString &response);
    void socketConnected();
    void ready();
    void socketDisconnected();
    void visionResult(const QString &result);
    void visionError(const QString &result);
    void visionInitOk();
    void visionInspecting();
    //void visionInspectingNok();
    void visionResultNok();
    void sendFailed(QString Data);

private:
    QString                             serverIp;
    quint16                             serverPort;
//    QMap<QByteArray,functor> protocolMap;
    bool								autoConnect;
    bool								autoStart;
    bool								autoReconnect;

    QTcpSocket                         *tcpSocket;
    QStateMachine                      *vision;
    QTimer                             *initTimer;
//    QTcpServer       *tcpServer;
//    QStringList      messageList = QString("Inspecting_ON;Inspecting_OFF;Wrong_model").split(";");
    QTimer                             *connectTimer;


};
#endif //SHERLOCK_MANAGER_H
