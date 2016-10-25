//*******************************************************************************

#ifndef KETCP_CLIENT_H
#define KETCP_CLIENT_H

#include <QObject>

#include <QTcpSocket>
#include <QDateTime>
#include <QBitArray>
#include <QTimer>


//*******************************************************************************
struct ConnectionParams
{  
    quint16 remoteport;
    QString remotehost;
    qint16 timeout; // in sec
    QString pass;
};

//*******************************************************************************
class KeTCP_Client: public QObject
{
    Q_OBJECT
public:
    KeTCP_Client();
    ~KeTCP_Client();
    void setConnectionParams(QString sHost, quint16 uiPort,QString pass);
    bool Disconnect();
    bool IsSocketConnected();
    void AutoRefreshStart(); // keSend("$KE,DAT,ON")
    void AutoRefreshStop(); // keSend("$KE,DAT,OFF")
    void WriteOutputs(quint16 outputs);
    void WriteRelay(int relay, bool sate);
    void WriteOneRelay(int relay, bool sate);
    bool IsConnectionValid();
    bool WiatConnectionValid(int msec);
    bool WiatNextResponse();
    bool isHostCorrect();
    //bool WiatConnectionOk(int msec);
    //TODO: SetEvent(); //
    //TODO: SavMode(); //
    //TODO: ErrHappend(Qstring description); //
    //TODO: filter drebezg for signal inputschanged
    //WrongResp();
public slots:
    int  actResps(QString response);
    bool keSend(QString replay);
    void keConnect();
    void startConnecting();
    void stopConnecting();
    void checkConnection();
    void readInputs();
    void enableEvents();
    void enableAutoRefresh();
    void refreshSysTime();
    void setSysTime(int secs);
    void watchAdcStart(int channel=1, int valveIn=1, int valveOut=1, int resetIn=2, int delay=15, int limit=5, int interval=1);
    void watchAdcStop();
    void watchCodesStart();


signals:
    void DataReceived(QString response);
    //void loginPassed();
    void ConnectionOk();
    void OutputsWriten(int Output);
    void RelayWriten();
    void InputsChanged(int Input,bool Res);
    void InputsWasChanged(int Input,bool Res, int deltaTime);
    void WasEvents();
    void connected();
    void disconnected();
    void InputsCodeStable(int inputCode,int tcpDeviceId);
    void InputsSingleStable(int inputNum);
    void sysTimeWrited();
    void sysTimeUpdated();
    void sysTimeRewrited();
    void sysTimeBad();
    void AdcChange(int channel, float value);

private:
    //TODO it was test
    //int  ActResps(QString response);
    void KeRespsInit();

private slots:
    void Recv();


private:
    QTcpSocket       *tcpSocket;
    ConnectionParams par;
    QStringList      KeList;
    bool             ConnectionValid;
    bool             ConnectionIsOk;
    bool             AutoInputs;
    bool             AutoRefresh;
    bool             AutoEvent;
    bool             AutoReconnect;
    bool             evtEnabled;
    QDateTime        KeSysStart;

    QBitArray           inputs;
    int                 inputCode;
    QTimer              watchAdcTimer;
    QTimer              sysTimeTimer;
    QTimer              icsTimer;
    QTimer              issTimer;
    QTimer              connectTimer;
    bool                outputs[12];
    bool                relay[4];
    quint32             counter[4];
    float               Adc[2];
    float               AdcZero[2];
    int                 delayADC;
    int                 limitADC;

};
#endif  //KETCP_CLIENT_H
//*******************************************************************************
//*******************************************************************************
