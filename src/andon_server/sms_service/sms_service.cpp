#include "sms_service.h"

void Sms_service::NetworkRequest(const QString &RequestURL, const int &SmsLogId, const QString &SmsInfo, const int &IdSms, const int &Iteration)
{
    QTimer * nrTimer = new QTimer;
    nrTimer->setSingleShot(true);
    QObject::connect(nrTimer,&QTimer::timeout,[=](){
        QNetworkReply * reply = manager->get(QNetworkRequest(QUrl(RequestURL)));
        reply->setProperty("SmsLogId",SmsLogId);
        reply->setProperty("SmsInfo",SmsInfo);
        reply->setProperty("IdSms",IdSms);
        reply->setProperty("Iteration",Iteration);
        //qDebug()<< "reuestsCount" << reuestsCount;
        nrTimer->deleteLater();
    });
    nrTimer->start(reuestsCount*3000);
    ++reuestsCount;
    //qDebug()<< "++reuestsCount" << reuestsCount;
}

Sms_service::Sms_service()
{
    manager = new QNetworkAccessManager;
    stURL="http://10.208.98.101:81/querymsg?user=SMS1&passwd=smssms1&apimsgid=%1";
    isSmsId.setPattern("^ID:\\s(\\d+)\n$");
    isSmsStatus.setPattern("^ID:\\s(\\d+)\\sStatus:\\s(\\d+)$");
    isErr.setPattern("^Err:\\s(\\d+)\n$");
    reuestsCount=0;
    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply){

        int SmsLogId = reply->property("SmsLogId").toInt();
        QString SmsInfo = reply->property("SmsInfo").toString();
        int IdSms = reply->property("IdSms").toInt();
        int Iteration = reply->property("Iteration").toInt();
        if (reply->error()>0)
            //qDebug() << "QNetworkReply error" << reply->error() << reply->errorString().replace("\n","")
            //         << "when try send SMS"
            //         << "SmsLogId:" << SmsLogId << IdSms
            //         << SmsInfo;

        if (reply->bytesAvailable()>0) {
            QByteArray result=reply->readAll();
            //qDebug() << "Reply result:" << result;
            isSmsId.indexIn(result);
            if (isSmsId.exactMatch(result)) {
                //qDebug() << "ID reply"
                //         << "SMS LOG ID:" << SmsLogId
                //         << "SMS ID:" << isSmsId.cap(1).toInt();
                SmsStatusUpdate(SmsLogId, isSmsId.cap(1).toInt(), -1);

                QTimer * stTimer = new QTimer;
                stTimer->setSingleShot(true);
                stTimer->setProperty("IdSms",isSmsId.cap(1).toInt());
                QObject::connect(stTimer,&QTimer::timeout,[=](){
                    NetworkRequest(stURL.arg(stTimer->property("IdSms").toInt()),
                                   SmsLogId,SmsInfo,stTimer->property("IdSms").toInt());
                    stTimer->deleteLater();
                });
                stTimer->start(30000);
            } else {
                isSmsStatus.indexIn(result);
                if (isSmsStatus.exactMatch(result)) {
                    IdSms=isSmsStatus.cap(1).toInt();
                    int StatusSms=isSmsStatus.cap(2).toInt();
                    if ((StatusSms==2 || StatusSms==3) && Iteration<3) {
                        Iteration++;
                        QTimer * st2Timer = new QTimer;
                        st2Timer->setSingleShot(true);
                        st2Timer->setProperty("IdSms",IdSms);
                        st2Timer->setProperty("Iteration",Iteration);
                        QObject::connect(st2Timer,&QTimer::timeout,[=](){
                            NetworkRequest(stURL.arg(st2Timer->property("IdSms").toInt()),
                                           SmsLogId,SmsInfo,st2Timer->property("IdSms").toInt(),
                                           st2Timer->property("Iteration").toInt());
                            st2Timer->deleteLater();
                        });
                        st2Timer->start(30000);
                    }
                    //qDebug() << "Status reply"
                    //         << "SMS LOG ID:" << SmsLogId
                    //         << "SMS ID:" << IdSms
                    //         << "Status:" << StatusSms
                    //         << "Iteration:" << Iteration;
                    SmsStatusUpdate(SmsLogId, IdSms, StatusSms);
                } else {
                    isErr.indexIn(result);
                    if (isErr.exactMatch(result)) {
                        qDebug() << "Error:" << isErr.cap(1).toInt()
                                 << " when try send SMS"
                                 << "SmsLogId:" << SmsLogId << SmsInfo;
                        SmsStatusUpdate(SmsLogId, IdSms, isErr.cap(1).toInt());
                    } else {
                        qDebug() << "Wrong QNetworkReply (" << result
                                 << ") when try send SMS"
                                 << "SmsLogId:" << SmsLogId << SmsInfo;
                        SmsStatusUpdate(SmsLogId, IdSms, -1);
                    }
                }
            }
        }
        reply->deleteLater();
        if (reuestsCount>0)
            --reuestsCount;
        //qDebug()<< "finish --reuestsCount" << reuestsCount;
    });


}

Sms_service::~Sms_service()
{
}


//TODO: class review
void Sms_service::English_SMS(QString &SMS_string,QextSerialPort *port)
{
    //English SMS
    port->write("ATZ\r\n");
    qDebug() << "ATZ\r\n";
    //ReadResponse(port);
    port->write("AT+CFUN=1\r\n");
    qDebug() << "AT+CFUN=1\r\n";
    //ReadResponse(port);
    port->write("AT+CMGF=1\r\n");
    qDebug() << "AT+CMGF=1\r\n";
    //ReadResponse(port);
    port->write("AT+CMGS=\"+79202710922\"\r\n");
    qDebug() << "AT+CMGS=\"+79202710922\"\r\n";
    //ReadResponse(port);
    port->write(QByteArray(SMS_string.toLocal8Bit()));
    qDebug() << SMS_string;
    //    qDebug() << "Hello Test SMS\r\n";
    //ReadResponse(port);
    port->write("\r\n\x1A");
    qDebug() << "\r\n\x1A";
    //ReadResponse(port);

    //    port->write("AT+CSCA=\"+79037011111\"");
    //    qDebug() << "AT+CSCA=\"+79037011111\"";
    //    ReadResponse(port);
}
bool Sms_service::Sendcommand(QextSerialPort *port,QByteArray QBA_ATCommand,QString ErrMsg,int DelaymSec)
{
    //int DelaymSec=2000;
    //QThread::msleep(DelaymSec);
    port->write(QBA_ATCommand);
    //    qDebug() << QBA_ATCommand;

    QThread::msleep(DelaymSec);
    char buff[1024];
    int numBytes;
    numBytes = port->bytesAvailable();
    if(numBytes > 0)
    {
        if(numBytes > 1024)
            numBytes = 1024;
        int i = port->read(buff, numBytes);
        buff[i] = '\0';
        QString msg = buff;
        //qDebug() << msg;
        if (msg.contains("OK")|msg.contains(">")|(msg.length()<5)) {/*qDebug() << msg;*/ return false;}
        else {qDebug() << ErrMsg << " : "  << msg; return true;}
        //TODO: if not resp from modem, wait 2 sec, cut DelaymSec arg.
    }
}
QByteArray Sms_service::PhoneConvert(QByteArray number)
{
    number += "F";
    QByteArray result = "0B"; // длина номера
    //QString().number(number.length(),16);

    result += "91" ; // международный формат номера
    int i = 0;
    while (i < number.length())
    {
        result += number[i + 1];
        result += number[i];
        i += 2;
    }
    return result;
}
QByteArray Sms_service::AaciiToUcs2(QString AnsiStr)
{
    QByteArray BAmsg;
    QTextCodec *codec2 = QTextCodec::codecForName("cp1251");
    BAmsg =codec2->fromUnicode(AnsiStr);
    QByteArray result;
    int i,len,Charcode,cd;
    len= AnsiStr.length();
    for (i=0;i<len;i++)
    {
        Charcode=(BAmsg[i] & 0xFF);
        cd = Charcode;
        if ((Charcode>=192) && (Charcode<=255)) cd = Charcode + 848;
        if  (Charcode ==184) cd = 1105;  // ё
        if  (Charcode ==168) cd = 1025;  // Ё
        result.append(QString("%1").arg(cd , 4, 16, QChar('0')).toUpper());
    }
    return result;
}
QByteArray Sms_service::PDUMessageConvert(QString PhoneNumber, QString Message)
{
    //    00 - всегда - это означает, что телефон/модем берет адрес сервис-центра SMS из Симки
    //    11 - всегда
    //    00 - указывает, что в качество отправителя будет указан собственный номер тел/модема
    //    0B - длина цифр телефонного номера = 11(дес)
    //    91 - указывает на интернациональный формат тел. номера (+7...)
    QByteArray result = "001100";
    int len;
    result += PhoneConvert(PhoneNumber.toLatin1()).append("0008C1");
    len=Message.length(); // Получим длину сообщения
    result.append(QString("%1").arg(len*2 , 2, 16, QChar('0')).toUpper());
    //    qDebug() << "Message length = " << len << "\r";
    // Дальше начинается перекодирование ASCII в PDU формат.
    result.append(AaciiToUcs2(Message));
    return result;
}
bool Sms_service::ishuawei (QString OutPortName) {
    QString portName;
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    int counter=0;
    //Navigate through ports untill you find huawei USB dongle
    while(counter<ports.size())
    {
        portName = ports[counter].portName;
        int productId= ports[counter].productID;
        //QString productId= ports[counter].productID;
        QString physicalName = ports[counter].physName;
        int vendorId = ports[counter].vendorID;
        //QString vendorId = ports[counter].vendorID;
        QString friendName = ports[counter].friendName;
        QString convertedPortName = portName.toLocal8Bit().constData();
        int convertedProductId = productId;
        //QString convertedProductId = productId.toLocal8Bit().constData();
        QString convertedPhysicalName = physicalName.toLocal8Bit().constData();
        int convertedVendorId = vendorId;
        //QString convertedVendorId = vendorId.toLocal8Bit().constData();
        QString convertedFriendName = friendName.toLocal8Bit().constData();
        counter++;
        //Break if you found Huwawei USB dongle, assign the port to a new port
        if (convertedFriendName.contains("HUAWEI Mobile Connect - 3G PC UI Interface"))
        {
            //port = new QextSerialPort(portName);
            OutPortName= portName;
            return true;
            //break;
        }
    }
    return false;
}



int Sms_service::SendSMS(const QString &PhoneNumber, const QString &Sms_text, const QString &Lang_name, const int &SmsLogId, int attempt)
//int Sms_service::SendSMS(QString PhoneNumber, QString MachineName_en, QString MachineName_ru, QString status_en, QString status_ru, QString datetime, QString Lang_name, int attempt)
{
    QString Message;
    QextSerialPort *port;
    QString portname;

    if (PhoneNumber[0] != '8') { return 0; }
    //TODO: listen of new USB OS event
    if (ishuawei(portname)) {
        if (port == (QextSerialPort*)0)    {
            //    port->setBaudRate(BAUD19200);
            //    port->setFlowControl(FLOW_OFF);
            //    port->setParity(PAR_NONE);
            //    port->setDataBits(DATA_8);
            //    port->setStopBits(STOP_2);

            //Write and send the SMS
            port->open(QIODevice::ReadWrite) ;
            //qDebug() << port->isOpen() << endl;
            //Russian SMS
            bool ErrFlg=false;
            //int PDU_mode=1;
            //TODO: loop with list of args of send comand, list of err msg
            //    ErrFlg = ErrFlg || Sendcommand(port,QByteArray("ATE0\r"),"Error set ATE0\r",2000);
            ErrFlg = ErrFlg || Sendcommand(port,QByteArray("AT+CFUN=1\r"),"Error set AT+CFUN=1\r",2000);  //modem initialization
            //    ErrFlg = ErrFlg || Sendcommand(port,QByteArray("ATZ\r"),"Error set ATZ\r",2000);
            ErrFlg = ErrFlg || Sendcommand(port,QByteArray("AT+CSCS=\"UCS2\"\r"),"Error set AT+CSCS=\"UCS2\"\r",2000);
            int PDU_mode;
            if (Lang_name=="EN")  PDU_mode=0; else  PDU_mode=1;
            ErrFlg = ErrFlg || Sendcommand(port,QByteArray("AT+CMGF=").append(QString::number(PDU_mode)).append("\r"),QByteArray("Error set AT+CMGF=").append(PDU_mode).append("\r"),2000);
            if (Lang_name=="EN")
            {
                //PDU_mode=0;
                //Message= MachineName_en.append(status_en).append(datetime);
                Message=Sms_text;
                QByteArray PDUMessage=PDUMessageConvert(PhoneNumber, Message);
                ErrFlg = ErrFlg || Sendcommand(port,QByteArray( "AT+CMGS=").append(QString::number((PDUMessage.length()-1)>>1,10)).append( "\r\n"),QString("Error set AT+CMGS= ").append(QString::number((PDUMessage.length()-1)>>1,10)).append( "\r\n"),2000);
                ErrFlg = ErrFlg || Sendcommand(port,QByteArray(PDUMessage).append("\x1A"),"An error occurred while sending SMS",3000);
            }
            else
            {
                //PDU_mode=1;
                //Message=MachineName_ru.append(status_ru).append(datetime);
                Message=Sms_text;
                ErrFlg = ErrFlg || Sendcommand(port,QByteArray("AT+CSMP=17,167,0,25"),"Error set AT+CSMP=17,167,0,25",2000);
                QString strPhoneNumber=AaciiToUcs2(strPhoneNumber);
                ErrFlg = ErrFlg || Sendcommand(port,QByteArray( "AT+CMGS=\"").append(strPhoneNumber).append( "\"\r\n"),QString("Error set AT+CMGS= \"").append(strPhoneNumber).append( "\"\r\n"),2000);
                ErrFlg = ErrFlg || Sendcommand(port,QByteArray("005500540046002D0038").append("\x1A"),"An error occurred while sending SMS",3000);
                //ErrFlg = ErrFlg || Sendcommand(port,QByteArray(AaciiToUcs2(Message)).append("\x1A"),"An error occurred while sending SMS",3000);
            }
            ErrFlg = ErrFlg || Sendcommand(port,QByteArray("0030\x1A"),"An error occurred while sending SMS",3000);
            if (ErrFlg) {
                //qDebug() << "SMS is not sent";
                return 0;
                //TODO: try few times, check code of step with err.
            }
            else {
                //qDebug() << "SMS sent";
                return 1;
            }
            port->flush();
            port->close();
            //qDebug() << port->isOpen() << endl;
            //system("pause");

        } else {
            //TODO: call sendSMSFECT if port did not open
            return 0;
        }
    }
    else {
        //qDebug() << "USB modem not found!";
        //return 0;
        //int SMSFECT_resp = sendSMSFECT(PhoneNumber, MachineName_en,MachineName_ru,status_en, status_ru, datetime, Lang_name,0);
        return 0; //sendSMSFECT(PhoneNumber, Sms_text, Lang_name, 0, 0);
    }
}
//TODO: make class fect sms server tcp sms sender
//TODO: read https://www.multitech.com/manuals/s000461f.pdf


void Sms_service::sendSMSFECT(const QString &PhoneNumber, const QString &Sms_text,
                              const QString &Lang_name, const int &SmsLogId, int attempt)
{
    if ((PhoneNumber[0] != '8') || (attempt>2)) { return; }
    int enc;
    QString SMSText;
    if (Lang_name=="EN"){
        enc = 1;
        SMSText =Sms_text;
    }
    else {
        enc = 3;
        SMSText=QString(ConvertStringToDecimalPoints(Sms_text));
    }
    qDebug() << QDateTime::currentDateTime().toString("dd.MM HH.mm.ss")
             << "SmsLogId:" << SmsLogId
             << "SMS:" << PhoneNumber << Sms_text << "enc"<< enc;

    NetworkRequest(URL.arg(enc).arg(PhoneNumber).arg(SMSText),SmsLogId,
                   QString().append(PhoneNumber).append(" ").append(Sms_text));

    //    QNetworkReply * reply = manager->get(QNetworkRequest(QUrl(URL.arg(enc).arg(PhoneNumber).arg(SMSText))));
    //    reply->setProperty("SmsLogId",SmsLogId);
    //    reply->setProperty("SmsInfo",QString().append(PhoneNumber).append(" ").append(Sms_text));
    //    QObject::connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>
    //                     (&QNetworkReply::error), [=](QNetworkReply::NetworkError code){
    //        qDebug() << "QNetworkReply error" << code << reply->errorString()
    //                 << "when try send SMS" << reply->property("SmsInfo").toString();
    //        reply->deleteLater();
    //    });

}
QByteArray Sms_service::ConvertStringToDecimalPoints(QString asciiString)
{
    //TODO: join from 2 to 1 function
    QByteArray BAmsg;
    QTextCodec *codec2 = QTextCodec::codecForName("cp1251");
    BAmsg =codec2->fromUnicode(asciiString);
    QByteArray result;
    int i,len,Charcode,cd;
    len= asciiString.length();
    for (i=0;i<len;i++)
    {
        Charcode=(BAmsg[i] & 0xFF);
        cd = Charcode;
        if ((Charcode>=192) && (Charcode<=255)) cd = Charcode + 848;
        if  (Charcode ==184) cd = 1105;  // ё
        if  (Charcode ==168) cd = 1025;  // Ё
        result.append(QString("%1").arg(cd , 4, 10, QChar('0')).toUpper()).append(";");
    }

    return result;
}

void Sms_service::setTcpSenderIP(const QString &IP_address)
{
    Tcp_sender_ip=IP_address;
}

void Sms_service::setTcpSenderPort(const QString &Port)
{
    Tcp_sender_port=Port;
}
void Sms_service::setTcp_sender_passwd(const QString &Passwd)
{
    Tcp_sender_passwd=Passwd;
}
void Sms_service::setURL(const QString &connectURL)
{
    URL=connectURL;
}


