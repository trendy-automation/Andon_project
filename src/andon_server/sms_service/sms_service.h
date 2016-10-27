#ifndef SMS_SERVICE_H
#define SMS_SERVICE_H

#include "qextserialport_global.h"
#include "qextserialport.h"
#include "qextserialenumerator.h"

#include <QtNetwork>
#include <QNetworkReply>
#include <QObject>

class Sms_service:public QObject
{
        Q_OBJECT
public:
    Sms_service();
    ~Sms_service();

public slots:
    int SendSMS(const QString &PhoneNumber, const QString &Sms_text, const QString &Lang_name,const int &SmsLogId, int attempt=0);
    void sendSMSFECT(const QString &phone, const QString &Sms_text, const QString &Lang_name,const int &SmsLogId, int attempt=0);
    void setTcpSenderIP(const QString &IP_address);
    void setTcpSenderPort(const QString &Port);
    void setTcp_sender_passwd(const QString &Passwd);
    void setURL(const QString &connectURL);

protected slots:
    void NetworkRequest(const QString &RequestURL, const int &SmsLogId, const QString &SmsInfo, const int &IdSms=0, const int &Iteration=0);

private:
    void English_SMS(QString &SMS_string,QextSerialPort *port);
    bool Sendcommand(QextSerialPort *port,QByteArray QBA_ATCommand,QString ErrMsg,int DelaymSec);
    QByteArray PhoneConvert (QByteArray number);
    QByteArray AaciiToUcs2(QString AnsiStr);
    QByteArray PDUMessageConvert(QString PhoneNumber, QString Message);
    QByteArray ConvertStringToDecimalPoints(QString asciiString);
    bool ishuawei (QString OutPortname);

signals:
    void SmsStatusUpdate(int SmsLogId, int SmsId, int Status);

private:
    QString  Tcp_sender_ip;   //10.208.98.101
    QString  Tcp_sender_port; //81
    QString  Tcp_sender_passwd;
    QString  URL;
    QString  stURL;
    QNetworkAccessManager *manager;
    QRegExp isSmsId;
    QRegExp isSmsStatus;
    QRegExp isErr;
    int reuestsCount;

};




#endif // SMS_SERVICE_H
