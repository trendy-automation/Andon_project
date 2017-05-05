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

    Q_PROPERTY(QString DEVICE_NAME READ getDeviceName WRITE setDeviceName)
    Q_PROPERTY(quint16 ID_TCPDEVICE MEMBER idDevice WRITE setIdDevice)
    Q_PROPERTY(QString TCPDEVICE_IP MEMBER deviceIp WRITE setDeviceIp)
    Q_PROPERTY(quint16 PORT MEMBER port WRITE setPort)
    Q_PROPERTY(QString PASS MEMBER pass WRITE setPass)
    Q_PROPERTY(QString AUX_PROPERTIES_LIST WRITE setAuxProperties)
    Q_PROPERTY(bool isSocketConnected READ isSocketConnected)


public:
    SherlockManager(QObject * parent=0);
    ~SherlockManager();

    void setDeviceName(const QString &devName) {this->setObjectName(devName);}
    QString getDeviceName() {return this->objectName();}

    void setIdDevice(quint16 tcpIdDevice) {idDevice=tcpIdDevice;}
    void setDeviceIp(const QString &tcpDevIp){if (!isSocketConnected()) {deviceIp=tcpDevIp; startConnecting();}}
    void setPort(quint16 tcpServerPort){if (!isSocketConnected()) {port=tcpServerPort; startConnecting();}}
    void setPass(const QString &devPass)     {if (!isSocketConnected()) {pass=devPass;      startConnecting();}}
    void setAuxProperties(const QString &auxPropertiesList);

    bool isSocketConnected() {return tcpSocket->state() == QAbstractSocket::ConnectedState;}
    void setAutoReconnect(bool connectNow=true){autoReconnect=connectNow;}
    void setAutoConnect(bool keepConnect=true){autoConnect=keepConnect;}

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
    quint16                             idDevice;
    QString                             deviceIp;
    quint16                             port;
    QString                             login;
    QString                             pass;

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
