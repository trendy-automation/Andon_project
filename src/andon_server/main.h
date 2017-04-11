#ifndef MAIN_H
#define MAIN_H

//#include <QFile>

//_______Excel Lib_______________
#include "xlsxdocument.h"

#include <QSqlQuery>
#include <QDebug>
#include <QTimer>
#include <QJsonDocument>
#include <QApplication>

//#include "qjsonrpctcpserver.h"
#include "server_rpcservice.h"
#include "sms_service.h"
#include "bcsender.h"


template<class T>
void listenPort(T * obj, int port, int interval, int delay) {
    QTimer *listenPortTimer = new QTimer;
    QObject::connect(listenPortTimer,&QTimer::timeout,[obj,port,listenPortTimer,interval](){
        if (obj->listen(QHostAddress::AnyIPv4, port)) {
            qDebug()<<QString("%1: %2 port opened").arg(obj->objectName()).arg(QString::number(port));
            listenPortTimer->stop();
            listenPortTimer->deleteLater();
        } else {
            qDebug()<<QString("%1: Failed to open port %2").arg(obj->objectName()).arg(port);
            listenPortTimer->start(interval);
        }
    });
    listenPortTimer->start(delay);
}

static void appClientConnected(const QHostAddress &clientIP)
{
    qDebug()<<"clientConnected"<<clientIP.toString();
    QJsonObject joClient;
    joClient.insert("STATION_IP", clientIP.toString());
    joClient.insert("EVENT_ID", QTime::currentTime().toString("HH:mm:ss.zzz"));
    joClient.insert("STATUS", "CONNECTED");
    QJsonDocument jdClient(joClient);
    ServerRpcService*andonRpcService=qApp->findChild<ServerRpcService*>("andonRpcService");
    if(andonRpcService)
        andonRpcService->StartSms(jdClient.toJson(QJsonDocument::Compact));
    else
        qDebug()<<"object andonRpcService not found in App";
}

static void appClientDisconnected(const QHostAddress &clientIP)
{
    qDebug()<<"clientDisconnected"<<clientIP.toString();
    QJsonObject joClient;
    joClient.insert("STATION_IP",clientIP.toString());
    joClient.insert("EVENT_ID",QTime::currentTime().toString("HH:mm:ss.zzz"));
    joClient.insert("STATUS", "DISCONNECTED");
    QJsonDocument jdClient(joClient);
    ServerRpcService*andonRpcService=qApp->findChild<ServerRpcService*>("andonRpcService");
    if(andonRpcService)
        andonRpcService->StartSms(jdClient.toJson(QJsonDocument::Compact));
    else
        qDebug()<<"object andonRpcService not found in App";
}

static void appParseInput(const QString &text)
{
    qDebug()<<"recived text:" << text;
    BCSender*bcSender=qApp->findChild<BCSender*>("bcSender");
    if(!bcSender){
        qDebug()<<"object bcSender not found in App";
        return;
    }
    if((text.left(5).compare("RENEW",Qt::CaseInsensitive)==0) && text.length()>7){
        QString ClientIPstr =text.toUpper().mid(6);
        qDebug() << "ClientIPstr:" << ClientIPstr;
        if(ClientIPstr=="ALL")
            bcSender->renewInterface();
        else
            bcSender->renewInterface(ClientIPstr); //TODO check ip
    }
}

static bool appCreateReport(QSqlQuery *query, const QString &reportName,const QString &fileName)
{
    QXlsx::Document * xlsx= new QXlsx::Document(QString(fileName).append(".xlsx"));
    if(xlsx->selectSheet(reportName))
        xlsx->deleteSheet(reportName);
    xlsx->addSheet(reportName);
    xlsx->selectSheet(reportName);
    int i=1;
    QTextCodec *codec = QTextCodec::codecForName("iso8859-1");
    for(int j=0; j < query->record().count(); j++)
        xlsx->write(i,j+1,QString(codec->fromUnicode(query->record().fieldName(j))));
    while(query->next()) {
        i++;
        for(int j=0; j < query->record().count(); j++)
            xlsx->write(i,j+1,query->value(j));
    }
//    if(i>1){
        if(xlsx->save()){
            qDebug()<<fileName<<"save OK";
            return true;
        }
        else
            qDebug()<<fileName<<"not saved";
//    }
//    else
//        xlsx->deleteLater(); //Is need?
    return false;
}

static void appAddbcClients(QSqlQuery *query)
{
    BCSender*bcSender=qApp->findChild<BCSender*>("bcSender");
    if(!bcSender){
        qDebug()<<"object bcSender not found in App";
        return;
    }
    while(query->next())
        bcSender->addClient(query->value(0));
}

#endif // MAIN_H
