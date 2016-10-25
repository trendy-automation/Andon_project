

#include "KeTCP_Client.h"
#include <QHostAddress>
#include <QTimer>
#include <QDateTime>
#include <QtCore/QDebug>
#include <QDataStream>
//#include <QDebug>
//#include <QtDebug>

#include <QTcpServer>
#include <QTcpSocket>

//________KEEPALIVE______
//#include <sys/socket.h>
//#include <netinet/tcp.h>
//#include <netinet/in.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <math.h>
#define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4)


//*******************************************************************************
KeTCP_Client::KeTCP_Client( )
{
    ConnectionValid=false;
    ConnectionIsOk=false;
    AutoInputs=true;
    AutoRefresh=false;
    AutoEvent=true;
    AutoReconnect=true;
    //evtEnabled=false;
    tcpSocket = new QTcpSocket(this);

    QObject::connect(this, &KeTCP_Client::connected, this, &KeTCP_Client::readInputs);
    QObject::connect(this, &KeTCP_Client::sysTimeWrited, this, &KeTCP_Client::enableEvents);
    QObject::connect(this, &KeTCP_Client::sysTimeBad, this, &KeTCP_Client::refreshSysTime);

    QObject::connect(tcpSocket, &QTcpSocket::readyRead, this, &KeTCP_Client::Recv);
    QObject::connect(tcpSocket, &QTcpSocket::stateChanged, this,
                     [=](){
//        qDebug() << this->objectName() << tcpSocket->state();
        });
//    QObject::connect(tcpSocket,  &QTcpSocket::disconnected, this, &KeTCP_Client::disconnected);
    QObject::connect(tcpSocket,  &QTcpSocket::disconnected, this,
                     [=](){
        emit this->disconnected();
//        qDebug() << this->objectName() << "tcpSocket disconnected";
        //QObject::disconnect(&aliveTimer,&QTimer::timeout,this,&KeTCP_Client::checkConnection);
        KeSysStart=QDateTime::fromMSecsSinceEpoch(0);
        inputs.clear();

        if (AutoReconnect)
            startConnecting();
        });

    sysTimeTimer.setInterval(2000);

//    aliveTimer.setSingleShot(false);
//    aliveTimer.setInterval(KE_ALIVE_TIMEOUT);

//    if (AutoReconnect) {
//        QObject::connect(&aliveTimer,&QTimer::timeout,this,&KeTCP_Client::checkConnection);
//    }

    KeRespsInit();
    //inputs.resize(6);
    //inputs.fill(0);

    connectTimer.setInterval(KE_RECONNECT_TIMEOUT);
    connectTimer.setSingleShot(false);
    QObject::connect(&connectTimer,&QTimer::timeout,this,&KeTCP_Client::keConnect);

    QObject::connect(tcpSocket,&QTcpSocket::connected,[=](){
        //inputs.resize(6);
        stopConnecting();

        struct tcp_keepalive {
        u_long onoff;
        u_long keepalivetime;
        u_long keepaliveinterval;
        };

        DWORD dwError = 0L,dwBytes ;
        tcp_keepalive pClSock_tcpKeepalive={0}, sReturned = {0};
        pClSock_tcpKeepalive.onoff=1;
        //enable keepalive
        pClSock_tcpKeepalive.keepalivetime=KE_ALIVE_TIMEOUT;
        // Every KE_ALIVE_TIMEOUT ms send pack
        pClSock_tcpKeepalive.keepaliveinterval=150;
        // If pack does not recieved in 1.5s send again
        pClSock_tcpKeepalive.keepaliveinterval=150;
 
        if (WSAIoctl(tcpSocket->socketDescriptor(), SIO_KEEPALIVE_VALS, &pClSock_tcpKeepalive,
        sizeof(pClSock_tcpKeepalive), &sReturned, sizeof(sReturned), &dwBytes,
        NULL, NULL) != 0)
        {dwError = WSAGetLastError() ;
        qWarning((char*)dwError); }


        //tcpSocket->waitForBytesWritten(15000);
        KeSysStart=QDateTime().fromMSecsSinceEpoch(0);
        keSend(QByteArray("$KE,PSW,SET,").append(par.pass));
        qDebug()<< this->objectName() << "Login to KBX100"
                << par.remotehost << ":" << par.remoteport <<"pass"<<par.pass;
        keSend("$KE,EVT,OFF");
        if (AutoRefresh)
            keSend("$KE,DAT,ON");
        if (AutoEvent)
            refreshSysTime();
        //qDebug()<< this->objectName() << "Read all inputs";
        if (AutoInputs)
            readInputs();
    });



//    issTimer.setSingleShot(true);
//    issTimer.setInterval(KE_WAITFOR_SABILE);
//    QObject::connect(&issTimer, &QTimer::timeout, this,
//                     [=](){emit InputsSingleStable(inputNum);
//                           qDebug() << "InputsSingleStable" << inputNum;
//                           });
//    QObject::connect(this,  &KeTCP_Client::InputsChanged,
//                     [=](int in, int res){
//        issTimer.stop();
//        issTimer.start();
//    });
}
//*******************************************************************************
void KeTCP_Client::setConnectionParams(QString sHost, quint16 uiPort , QString pass)
{
        par.remotehost = sHost;
        par.remoteport = uiPort;
        par.pass=pass;
}

//*******************************************************************************
KeTCP_Client::~KeTCP_Client()
{
    Disconnect();
}

//*******************************************************************************

bool KeTCP_Client::isHostCorrect()
{
    return !(par.remotehost.isEmpty() || par.remoteport==0);
}

/*bool KeTCP_Client::Reconnect()
{
    qDebug()<<"connecting to "<<this->objectName()<<"...";
    if (IsSocketConnected())  {
        if (tcpSocket->peerAddress().toString()!=par.remotehost) {
            Disconnect();
            qDebug() << this->objectName() << "Laurent disconnected";
        } else {return true;}
    }
    if (!(par.remotehost.isEmpty() || par.remoteport==0)){
        tcpSocket->abort();
        //tcpSocket->disconnect();
        tcpSocket->connectToHost(par.remotehost,par.remoteport);
        return true;
    } else {
        qDebug() << "Could not connect to Laurent on host: " << par.remotehost << ":" << par.remoteport;
        return false;
    }

}
*/

//*******************************************************************************


void KeTCP_Client::startConnecting()
{
    qDebug()<<"Start connecting to "<<this->objectName();
    keConnect();
    connectTimer.start();
}

void KeTCP_Client::stopConnecting()
{
//    qDebug()<<"Stop connecting to "<<this->objectName();
    connectTimer.stop();
}

void KeTCP_Client::keConnect()
{
    if (isHostCorrect()) {
//    qDebug()<<"Connecting to "<<this->objectName()<<"...";
    tcpSocket->abort();
    tcpSocket->connectToHost(par.remotehost,par.remoteport);

//    QTimer * ketimer= new QTimer;
//    QObject::connect(ketimer,&QTimer::timeout,this,&KeTCP_Client::Reconnect);
//    QObject::connect(this, &KeTCP_Client::ConnectionValidated, [=]() {
//            qDebug() << this->objectName() << " connection success";
//            QObject::disconnect(ketimer,&QTimer::timeout,this,&KeTCP_Client::Reconnect);
//            //tcpSocket->waitForDisconnected(5000);
//            ketimer->deleteLater();
//    });
//    ketimer->start();
    }
}

void  KeTCP_Client::checkConnection()
{
    keSend("$KE");
}

//*******************************************************************************
void KeTCP_Client::Recv()
{
    if (IsSocketConnected())  {
        QString response;
        QTextStream in(tcpSocket);
        while (!in.atEnd()) {
            response = in.readLine();
            actResps(response);
        }
        emit DataReceived(response);
    } else {
        //TODO: need to reconnect?
        qDebug() << "KE tcpSocket status:" << tcpSocket->state();
    }
}




//*******************************************************************************
bool KeTCP_Client::keSend(QString replay)
{
    if (tcpSocket->isOpen()){
        tcpSocket->write(replay.toLatin1().append("\r\n"));
    //    qDebug() << "KE replay send: "<< replay;
        return true;
    } else {
        qDebug() << "KE tcpSocket not opened";
        return false;
    }
}

//*******************************************************************************
bool KeTCP_Client::Disconnect( )
{
    ConnectionValid=false;
    ConnectionIsOk=false;
    if (IsSocketConnected()  )  {
        tcpSocket->disconnect();
        return true;
    } return false;
}

//*******************************************************************************
bool KeTCP_Client::IsSocketConnected( )
{
    return tcpSocket->state() == QAbstractSocket::ConnectedState;
}

//*******************************************************************************
int KeTCP_Client::actResps(QString response)
{
    //qDebug() << "KE response:" << response;
    QStringList  RespList=response.split(",");
    bool ok=true;
    switch(KeList.indexOf(RespList[0])){
    case 0:
        // Login OK/NOK
        if (RespList[2]=="OK") {
            //qDebug() << this->objectName() << "Login OK";
            ConnectionValid=true;
            //emit loginPassed();
            //emit connected();
        }else {
            ConnectionValid=false;
            qDebug() << "Bad KE password:" << par.pass;
        }
        break;
    case 1:
        // OK
        //qDebug() << "KE response OK";
        ConnectionIsOk=true; //valid - logined
        emit ConnectionOk();
        break;
    case 2:
        // ERR
        ConnectionIsOk=false;
        qDebug() << "Wrong KE syntax!";
        break;
    case 3:
        // DAT OK?
        if (RespList[1]=="OK")
            qDebug() << "DAT streaming on!";
        break;
    case 4:
        // WR OK?
        //        if (RespList[1]=="OK") ;
        //qDebug() << "Writing ok!";
        break;
    case 5:
        // WRA
        if (RespList[1]=="OK") emit OutputsWriten(RespList[2].toInt(&ok,10));
        break;
    case 6:
        // REL
        if (RespList[1]=="OK") emit RelayWriten();
        break;
    case 7:
        //SYSTEM TIME dec
        setSysTime(RespList[1].toInt(&ok));
        //KeSysStart = QDateTime(QDateTime::currentDateTime()).addSecs(-RespList[1].toInt(&ok));
        break;
    case 8:
        //RD inputs 1-6
//        qDebug()<< "inputs=" << RespList[1];
        if (inputs.isEmpty())
            inputs.resize(6);
        if (RespList[1]=="ALL") {
            for (int i=0;i<6;++i) {
                bool in_res =RespList[1].mid(i,1).toInt(&ok,2);
                if ((inputs[i]!=in_res) && ok) {
                    inputs[i]=in_res;
                    //qDebug()<< "in_changed" << in_changed << "inputs["<<i<<"]" <<inputs[i] <<"in_res"<<in_res;
                    //qDebug()<< "inputs[" << i <<"]="<<in_res;
                    emit InputsChanged(i,in_res);
                }
            }
        }
        break;
    case 9:
        //RID output 1-12
        if (RespList[1]=="ALL"){
            for (int i=0;i<12;++i) outputs[i]=RespList[2].mid(i,1).toInt(&ok,2);
        }
        break;
    case 10:
        //RDR relay 1-4
        if (RespList[1]=="ALL"){
            for (int i=0;i<4;++i) relay[i]=RespList[2].mid(i,1).toInt(&ok,2);
        }
        break;
    case 11:
        //ADC dec
        qDebug() << "ADC dec!" << RespList[1].toInt(&ok) << RespList[2].toFloat(&ok);
        int channel;
        channel = RespList[1].toInt(&ok);
        float value;
        value = RespList[2].toFloat(&ok);
        if (ok) {
            if (Adc[channel]!=value) {
                Adc[channel]=value;
                emit AdcChange(channel, value);
            }
        }
        break;
    case 12:
        //TMP temperature float
        break;
    case 13:
        //IMPL,1 counter 1 dec
        counter[RespList[1].toInt(&ok,10)]= RespList[5].toInt(&ok,10)*32766 + RespList[6].toInt(&ok,10);
        qDebug() << this->objectName() << "IMPL RESPONSE";
        break;
    case 14:
        //INT,1 counter 1 dec
        //counter[RespList[1].toInt(&ok,10)]= RespList[3].toInt(&ok,10)*32766 + RespList[4].toInt(&ok,10);
        //if (KeSysStart==QDateTime().fromMSecsSinceEpoch(0))
            setSysTime(RespList[3].toInt(&ok));
        break;
    case 15:
        //EVT
        //if (evtEnabled) {
//            if (RespList[1]=="OK")
//                qDebug() << this->objectName() << "Event whatchdog ok";
            if (RespList[1]=="IN") {
                if (!inputs.isEmpty())
                    inputs[RespList[3].toInt(&ok)-1]=RespList[4].toInt(&ok);
//                if (KeSysStart==QDateTime().fromMSecsSinceEpoch(0)) {
//                    QTimer *setKeTime;
//                    QDateTime * keSysTime;
         //           KeSysStart=QDateTime().currentDateTime().addSecs(-RespList[2].toInt(&ok));
//                    setKeTime->setInterval(1000);
//                    setKeTime->setSingleShot(true);
//                    QObject::connect(setKeTime, &QTimer::timeout, [=]() {
//                        if (KeSysStart==QDateTime().fromMSecsSinceEpoch(0))
//                            KeSysStart = *keSysTime;
//                        setKeTime->deleteLater();
//                    });

//                    setKeTime->start();
                              //<<  "Start time" << KeSysStart.toLocalTime().toString("dd.MM HH:mm:ss")
                              //<< "System time" <<RespList[2].toInt(&ok) ;//<< "("
                              //<< KeSysStart.addSecs(RespList[2].toInt(&ok)).toLocalTime().toString("dd.MM HH:mm:ss") <<")";
//                } else {
                int deltaTime=QDateTime::currentDateTime().secsTo(KeSysStart.addSecs(RespList[2].toInt(&ok)));
                if (KeSysStart==QDateTime::fromMSecsSinceEpoch(0) || abs(deltaTime)>5)
                    refreshSysTime();
                //??? обновлять когда нет событий в течении 10 секунд.
                //??? Или На время обновления считать все события актуальными


                if (sysTimeTimer.isActive()) {
//                    qDebug() << this->objectName() << "Systime" << RespList[2].toInt(&ok) << "Inputs was changed [IN"
//                             << RespList[3].toInt(&ok) << "]=" << RespList[4].toInt(&ok);
                    emit InputsWasChanged(RespList[3].toInt(&ok)-1,RespList[4].toInt(&ok), deltaTime);
                }
                else {
                    emit InputsChanged(RespList[3].toInt(&ok)-1,RespList[4].toInt(&ok));
                    //TODO: determinate delta old signals & shifts of time

                    if(abs(deltaTime)>2)
                        setSysTime(RespList[2].toInt(&ok));
                }

//                    QDateTime now = QDateTime::currentDateTime();
//                    int deltaTime = now.secsTo(KeSysStart.addSecs(RespList[2].toInt(&ok)));
//                    if (abs(deltaTime)<5){
//                        qDebug() << this->objectName() << "InputsChanged IN["
//                                 << RespList[3].toInt(&ok) << "]=" << RespList[4].toInt(&ok);
//                        emit InputsChanged(RespList[3].toInt(&ok)-1,RespList[4].toInt(&ok));
//                    } else {
//                        qDebug() << this->objectName() << "Inputs was changed IN["
//                                 << RespList[3].toInt(&ok) << "]=" << RespList[4].toInt(&ok);
//                        readInputs();
//                    }

//                    if (abs(deltaTime)>2) {
//                        if (abs(deltaTime)>5) {
//                            qDebug() << this->objectName() << "Bad System Time";
//                            KeSysStart=QDateTime().fromMSecsSinceEpoch(0);
//                            keSend("$KE,IMPL,1");
//                        } else {
//                            qDebug() << this->objectName() << "Adjustment of Time (" <<
//                                        KeSysStart.toLocalTime().toString("dd.MM HH:mm:ss") <<"deltaTime=" <<deltaTime;
//                            KeSysStart=KeSysStart.addSecs(-deltaTime);;
//                            qDebug() << "System Time " << KeSysStart.toLocalTime().toString("dd.MM HH:mm:ss");
//                        }
//                    }
//                }
            }
        //}
        break;
    case 16:
        //SLINF ?
        qDebug() << "Was Events";
        emit WasEvents();
        break;

    default:
        //TODO: advanced KE switch case
        qDebug() << "unknown KE response:" << response;
        return 0;
        break;
    }
    if (!ok) {qDebug() << "Bad KE response:" << response;return 0;} else
        return KeList.indexOf(RespList[0])+1;
}
//*******************************************************************************

void KeTCP_Client::setSysTime(int secs)
{
    QString res;
    QDateTime newTime = QDateTime::currentDateTime().addSecs(-secs);
    if (KeSysStart==QDateTime::fromMSecsSinceEpoch(0)) {
        res = "System time writed";
        emit sysTimeWrited();
    } else if (abs(newTime.secsTo(KeSysStart))>2 && abs(newTime.secsTo(KeSysStart))<5) {
        res = "System time updated";
        emit sysTimeUpdated();
    } else if (abs(newTime.secsTo(KeSysStart))>5) {
        res = "System time rewrited";
        emit sysTimeRewrited();
    } //else {
        //return;
    //}
//    if (!res.isEmpty())
//        qDebug() << this->objectName() << res << KeSysStart.toLocalTime().toString("dd.MM HH:mm:ss")
//                 << "to" << newTime.toLocalTime().toString("dd.MM HH:mm:ss");
    KeSysStart=newTime;
    sysTimeTimer.stop();
}

void  KeTCP_Client::KeRespsInit()
{
    QString strSimpleLine = "#PSW;#OK;#ERR;#DAT;#WR;#WRA;#REL;#TIME;#RD;#RID;#RDR;#ADC;#TMP;#IMPL;#INT;#EVT;#SLINF";
    KeList = strSimpleLine.split(";");
}

//*******************************************************************************
void KeTCP_Client::AutoRefreshStart()
{
    keSend("$KE,DAT,ON");
}
void KeTCP_Client::AutoRefreshStop()
{
    keSend("$KE,DAT,OFF");
}
void KeTCP_Client::WriteOutputs(quint16 outputs)
{
    keSend(QByteArray("$KE,WRA,").append((const char*)&outputs, sizeof(outputs)));
}
void KeTCP_Client::WriteRelay(int relay, bool sate)
{
    if ((relay>0) && (relay<5))
        keSend(QByteArray("$KE,REL,").append(QString::number(relay)).append(",").append(sate?"1":"0"));
}
//TODO overload with string list inputs
void KeTCP_Client::WriteOneRelay(int relay, bool sate)
{
    if ((relay>0) && (relay<5))
        for (int i=1;i<5;i++)
            keSend(QByteArray("$KE,REL,").append(QString::number(i))
                 .append(",").append((i==relay)?(sate?"1":"0"):"0"));
}
//*******************************************************************************
bool KeTCP_Client::IsConnectionValid()
{
  return ConnectionValid;
}

bool KeTCP_Client::WiatConnectionValid(int msec)
{
    return ConnectionValid;
    //TODO: return true immidetly then ConnectionValid==true;
    //QTimer -> msec ->  return false;
    //connect (this,ConnectionValidated,QTimer,disconnect)
    //QTimer::singleShot(msec, Qt::CoarseTimer, this, KeTCP_Client::IsConnectionValid);

}

bool KeTCP_Client::WiatNextResponse()
{
    //TODO What a fuck
    //if (nextresp)
        return true;
}

void KeTCP_Client::readInputs()
{
    //qDebug()<< this->objectName() << "Read all inputs";
    keSend("$KE,RD,ALL");
}

void KeTCP_Client::enableEvents()
{
    if (AutoEvent)
        keSend("$KE,EVT,ON");
}

void KeTCP_Client::enableAutoRefresh()
{
    if (AutoRefresh)
        keSend("$KE,DAT,ON");
}

void KeTCP_Client::refreshSysTime()
{
    //qDebug()<< "refreshSysTime";
    sysTimeTimer.start();
    keSend("$KE,IMPL,1");
}

void KeTCP_Client::watchAdcStart(int channel, int valveIn, int valveOut, int resetIn, int delay, int limit, int interval)
{
    watchAdcTimer.disconnect();
    QObject::connect(this,&KeTCP_Client::InputsChanged, [this,valveIn,delay](int input, bool res){
        if(((input==valveIn) && res) && (!watchAdcTimer.isActive()))
            watchAdcTimer.start(delay*1000);
        });
    QObject::connect(&watchAdcTimer, &QTimer::timeout, [this,channel,interval,limit,resetIn,valveOut](){
        keSend(QString().number(channel).prepend("$KE,ADC,")); 
        AdcZero[channel] = Adc[channel];
        if (AdcZero[channel]!=0)
            outputs[valveOut] = (abs(AdcZero[channel] - Adc[channel])*100/AdcZero[channel]>limit) && (not inputs[resetIn]);
        qDebug() << "valveOut =" << outputs[valveOut] << ";  Adc val =" <<Adc[channel];
        watchAdcTimer.setInterval(interval*1000);
        //watchAdcTimer.start(interval*1000);
        });
    watchAdcTimer.start(delay*1000);
}

void KeTCP_Client::watchAdcStop()
{
    watchAdcTimer.stop();
}

void KeTCP_Client::watchCodesStart()
{
    icsTimer.setSingleShot(true);
    icsTimer.setInterval(KE_WAITFOR_SABILE);
    QObject::connect(&icsTimer, &QTimer::timeout, this,
                     [=](){emit InputsCodeStable(inputCode,this->property("ID_TCPDEVICE").toInt());
//                           qDebug() << this->objectName() << "InputsCodeStable" << inputCode;
                           });
    QObject::connect(this,  &KeTCP_Client::InputsChanged,
                     [=](int InputNum, int InputVal){
//        qDebug() << this->objectName() << "InputsChanged IN["
//                 << InputNum+1 << "]=" << InputVal;
        icsTimer.stop();
        QByteArray byteCode("\x00");
        //byteCode.resize(1);
        //byteCode.fill(0);
        for (int i=0;i<inputs.count();++i)
            byteCode[0]=(byteCode.at(0)|((inputs[i]?1:0)<<i));
            //byteCode[i/8]=(byteCode.at(i/8)|((inputs[i]?1:0)<<(i%8)));
        inputCode=(int)byteCode.at(0);
        if (inputCode!=0)
            icsTimer.start();
    });
}

