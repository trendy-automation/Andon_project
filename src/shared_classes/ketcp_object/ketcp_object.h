//*******************************************************************************

#ifndef KETCP_OBJECT_H
#define KETCP_OBJECT_H

#include <QObject>

#include <QTcpSocket>
#include <QDateTime>
#include <QBitArray>
#include <QTimer>
#include <QDateTime>
#include <QHostAddress>
#include <functional>


//*******************************************************************************
//struct ConnectionParams
//{
//    quint16 remoteport;
//    QString remotehost;
//    quint16 timeout; // in sec
//    QString pass;
//};

//*******************************************************************************
struct AdcChannel
{
    QTimer trackingTimer; //timer to start tracking
    int channel;
    float value;
    float nominal;
    int delay;
    int tolerance; //percent
};

//*******************************************************************************
class KeTcpObject: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool input_1 READ getInput_1)
    Q_PROPERTY(bool input_2 READ getInput_2)
    Q_PROPERTY(bool input_3 READ getInput_3)
    Q_PROPERTY(bool input_4 READ getInput_4)
    Q_PROPERTY(bool input_5 READ getInput_5)
    Q_PROPERTY(bool input_6 READ getInput_6)
    Q_PROPERTY(bool input_7 READ getInput_7)
    Q_PROPERTY(bool input_8 READ getInput_8)
    Q_PROPERTY(bool input_9 READ getInput_9)
    Q_PROPERTY(bool input_10 READ getInput_10)
    Q_PROPERTY(bool input_11 READ getInput_11)
    Q_PROPERTY(bool input_12 READ getInput_12)
    Q_PROPERTY(bool isSocketConnected READ isSocketConnected)
//    Q_PROPERTY(bool isReady READ isReady) //NOTIFY ready
//    Q_PROPERTY(QBitArray inputs MEMBER inputs NOTIFY inputsChanged)
    Q_PROPERTY(quint32 inputCode MEMBER inputCode NOTIFY inputCodeChanged)
    Q_PROPERTY(QString DEVICE_NAME READ getDeviceName WRITE setDeviceName)
    Q_PROPERTY(quint16 ID_TCPDEVICE MEMBER idDevice WRITE setIdDevice)
    Q_PROPERTY(QString TCPDEVICE_IP MEMBER deviceIp WRITE setDeviceIp)
    Q_PROPERTY(quint16 PORT MEMBER port WRITE setPort)
//    Q_PROPERTY(QString LOGIN MEMBER login WRITE setLogin)
    Q_PROPERTY(QString PASS MEMBER pass WRITE setPass)
    Q_PROPERTY(int poewrOnSec READ getPoewrOnSecs)
    Q_PROPERTY(int poewrOnDays READ getPoewrOnDays)
    //Q_PROPERTY(bool CONNECT_AFTER_CONFIG MEMBER connectOnConfig) //TODO to avoid "Ke.startConnecting"
    //Q_PROPERTY(QBitArray outputs READ getOutputs WRITE setOutputs)
    //Q_PROPERTY(QBitArray relays READ getRelays WRITE setRelays)
    //Q_PROPERTY(AdcChannel.value adc MEMBER)
    //Q_PROPERTY(AdcChannel.deviation adc MEMBER)
//    AutoInputs=true;
//    AutoRefresh=false;
//    AutoEvent=true;
//    AutoReconnect=true;
//    drebezg
//    savingEnabled



public:
    KeTcpObject();
    ~KeTcpObject();

    void setDeviceName(const QString &devName) {this->setObjectName(devName);}
    void setIdDevice(quint16 tcpIdDevice) {idDevice=tcpIdDevice;}
    void setDeviceIp(const QString &tcpDevIp){if (!isSocketConnected()) {deviceIp=tcpDevIp; startConnecting();}}
    void setPort(quint16 devPort)            {if (!isSocketConnected()) {port=devPort;      startConnecting();}}
//    void setLogin(const QString &devLogin)   {if (!isSocketConnected()) {login=devLogin;    startConnecting();}}
    void setPass(const QString &devPass)     {if (!isSocketConnected()) {pass=devPass;      startConnecting();}}

    int getPoewrOnSecs(){if (isSocketConnected()) return KeSysStart.secsTo(QDateTime::currentDateTime()); else return -1;}
    int getPoewrOnDays(){if (isSocketConnected()) return KeSysStart.daysTo(QDateTime::currentDateTime()); else return -1;}
    QString getDeviceName() {return this->objectName();}

//    void setParams(QVariantMap params);
//    void setConnectionParams(QString sHost, quint16 uiPort,QString pass);
//    bool Disconnect();
    bool isSocketConnected() {return tcpSocket->state() == QAbstractSocket::ConnectedState;}
    void AutoRefreshStart(); // keSend("$KE,DAT,ON")
    void AutoRefreshStop(); // keSend("$KE,DAT,OFF")
    void WriteOutputs(quint16 outputs);
    void WriteRelay(int relay, bool sate);
    void WriteSingleRelay(int relay, bool sate);
//    bool IsConnectionValid();
//    bool WiatConnectionValid(int msec);
//    bool WiatNextResponse();
    //TODO: SetEvent(); //
    //TODO: SavMode(); //
    //TODO: ErrHappend(Qstring description); //
    //TODO: filter drebezg for signal inputschanged
    //WrongResp();
    bool getInput_1(){return inputs.testBit(1);}
    bool getInput_2(){return inputs.testBit(2);}
    bool getInput_3(){return inputs.testBit(3);}
    bool getInput_4(){return inputs.testBit(4);}
    bool getInput_5(){return inputs.testBit(5);}
    bool getInput_6(){return inputs.testBit(6);}
    bool getInput_7(){return inputs.testBit(7);}
    bool getInput_8(){return inputs.testBit(8);}
    bool getInput_9(){return inputs.testBit(9);}
    bool getInput_10(){return inputs.testBit(10);}
    bool getInput_11(){return inputs.testBit(11);}
    bool getInput_12(){return inputs.testBit(12);}
public slots:
//    void addIntInputMask(const QString &maskName,const QByteArray &mask);
    QVariantMap getProperties(const QStringList &requested=QStringList());
    void actResps(QString response);
    bool keSend(QString replay);

//    bool isConnected(){return isSocketConnected();}
    void emitIfReady(){ if((KeSysStart!=QDateTime::fromMSecsSinceEpoch(0)) && (!inputs.isEmpty())) emit ready();}
    void startConnecting(bool startTimer=true);
//    void stopConnecting();
//    void checkConnection();
    void readInputs(){ keSend("$KE,RD,ALL");}
    void readTemp(){ keSend("$KE,TMP");}

    void enableEvents(bool force=false) { if (AutoEvent || force) keSend("$KE,EVT,ON");}
    void enableRefreshing(bool force=false) {if (AutoRefresh || force) keSend("$KE,DAT,ON");}
//    void enableBounceOff(); //TODO
//    void enableSaving();    //TODO

    void refreshSysTime();
    void setSysTime(int secs);
    void setInputCodeSize(quint8 size){if(size<=32)inputCodeSize=size;}
//    void emitIOEvent(const QString &ioName,const QVariant &val);


    //deprecated
    void watchAdcStart(int channel=1, int valveIn=1, int valveOut=1, int resetIn=2, int delay=15, int limit=5, int interval=1);
    void watchAdcStop();
    void watchCodesStart();


signals:
    void connectionStateChanged(bool isConnected);
    void inputsChanged(const QBitArray &inputs);
    void inputCodeChanged(quint32 inputCode);
    void inputChanged(int InputNum, bool InputVal);
    void tempChanged(float temperature);

    void IOEvent(const QString &ioName,const QVariant &val);
    void DataReceived(const QString &response);
    void accessAllowed();
    void OutputsWriten(int Output);
    void RelayWriten();
    void inputsWasChanged(int Input,bool Res, int deltaTime);
    void wasEvents();
    void connected();
    void ready();
    void disconnected();
    void inputsCodeStable(int inputCode,int tcpDeviceId);
    void sysTimeWrited();
    void sysTimeUpdated();
    void sysTimeRewrited();
    void sysTimeBad();
    void AdcChange(int channel, float value);

private:
    void initIO(){
        inputCode=0;
        inputs.resize(0);
        outputs.resize(0);
        relays.resize(0);
        temperature=0;
    }

//    int getInputsCount();


private:

    //properties************************************************
    QBitArray                           inputs;
    quint32                             inputCode;
    quint16                             idDevice;
    QString                             deviceIp;
    quint16                             port;
    QString                             login;
    QString                             pass;

//    bool                                isReady;
//    bool                                connectOnConfig=true;

    //TODO properties
    QList<quint32>                      counters;
    QBitArray                           relays;
    QBitArray                           outputs;
    bool             eventEnabled;
    bool             bounceOffEnabled;
    bool             refreshingEnabled;
    bool             savingEnabled; //TODO $KE,SAV
//    AdcChannel.value
//    AdcChannel.deviation
    //************************************************8

    QTcpSocket       *tcpSocket;
//    ConnectionParams par;
    //TODO: translate to QMap<QByteArray,functor> //[](){}
    QStringList      KeList = QString("#PSW;#OK;#ERR;#DAT;#WR;#WRA;#REL;#TIME;#RD;#RID;#RDR;#ADC;#TMP;"
                                      "#IMPL;#INT;#EVT;#SLINF;#DATA_SAVE;#FLAGS;JConfig from FLASH;#;#EVT#EVT").split(";");
    QDateTime        KeSysStart;

    //Timers
    QTimer                              watchAdcTimer;
    QTimer                              sysTimeTimer;
    QTimer                              icsTimer;
//    QTimer                              issTimer;
    QTimer                              connectTimer;

    //deprecated
//    bool             ConnectionValid;
//    bool             ConnectionIsOk;
    bool             AutoInputs;
    bool             AutoRefresh;
    bool             AutoEvent;
    bool             AutoReconnect;
    bool                                relay[4];
    quint32                             counter[4];
    float                               Adc[2]; //TODO Jerome ADC auto bound
    float                               AdcZero[2];
    float temperature;
    float temperatur;
    float tmp;
    float tolerance=0.5;
    int                                 delayADC;
    int                                 limitADC;
    QMap<QString,QBitArray>                              intInputMask[10];
    QMap<QString,std::function<void(QStringList)>>  responseProcessingMap;
    quint8                              inputCodeSize=KE_MAX_INPUTCODE_SIZE;

};
#endif //KETCP_OBJECT_H
//*******************************************************************************
//*******************************************************************************
