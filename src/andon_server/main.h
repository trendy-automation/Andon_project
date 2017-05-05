#ifndef MAIN_H
#define MAIN_H

//#include <QFile>

//_______Excel Lib_______________
#include "xlsxdocument.h"
QTXLSX_USE_NAMESPACE

#include <QSqlQuery>
#include <QDebug>
#include <QTimer>
#include <QJsonDocument>
#include <QApplication>

//#include "qjsonrpctcpserver.h"
#include "server_rpcservice.h"
#include "sms_service.h"
#include "bcsender.h"
#include "dbwrapper.h"
#include "functional"
#include <QChar>
#include "common_functions.h"



static void appClientConnected(const QHostAddress &clientIP)
{
    qDebug()<<"appClientConnected()";
    qDebug()<<"clientConnected"<<clientIP.toString();
    QJsonObject joClient;
    joClient.insert("STATION_IP", clientIP.toString());
    joClient.insert("EVENT_ID", QTime::currentTime().toString("HH:mm:ss.zzz"));
    joClient.insert("STATUS", "CONNECTED");
    QJsonDocument jdClient(joClient);
    ServerRpcService*andonRpcService=cfGetObject<ServerRpcService>("andonRpcService");
    //qApp->findChild<ServerRpcService*>("andonRpcService");
    if(andonRpcService)
        andonRpcService->StartSms(jdClient.toJson(QJsonDocument::Compact));
    //else
    //    qDebug()<<"object andonRpcService not found in App";
}

static void appClientDisconnected(const QHostAddress &clientIP=QHostAddress::LocalHost)
{
    qDebug()<<"clientDisconnected"<<clientIP.toString();
    QJsonObject joClient;
    joClient.insert("STATION_IP",clientIP.toString());
    joClient.insert("EVENT_ID",QTime::currentTime().toString("HH:mm:ss.zzz"));
    joClient.insert("STATUS", "DISCONNECTED");
    joClient.insert("USER_COMMENT", clientIP.toString());
    QJsonDocument jdClient(joClient);

    ServerRpcService*andonRpcService=cfGetObject<ServerRpcService>("andonRpcService");
            //qApp->findChild<ServerRpcService*>("andonRpcService");

    if(andonRpcService)
        andonRpcService->StartSms(jdClient.toJson(QJsonDocument::Compact));
    //else
    //    qDebug()<<"object andonRpcService not found in App";
}

static void appParseInput(const QString &text)
{
    qDebug()<<"recived text:" << text;
    BCSender*bcSender=cfGetObject<BCSender>("bcSender");
    //qApp->findChild<BCSender*>("bcSender");
    if(!bcSender){
        //qDebug()<<"object bcSender not found in App";
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

bool appCreateReport(QSqlQuery *query, const QString &sheetName,const QString &fileName,const QString &ariaName="")
{
    Document * xlsx= new Document(QString(fileName).append(".xlsx"));
    if(xlsx->selectSheet(sheetName))
        xlsx->deleteSheet(sheetName);
    xlsx->addSheet(sheetName);
    int i=1;
    QTextCodec *codec = QTextCodec::codecForName("iso8859-1");
    for(int j=0; j < query->record().count(); j++)
        xlsx->write(i,j+1,QString(codec->fromUnicode(query->record().fieldName(j))));
    while(query->next()) {
        i++;
        for(int j=0; j < query->record().count(); j++)
            xlsx->write(i,j+1,query->value(j));
    }
    if(!ariaName.isEmpty())
        if(!xlsx->defineName(ariaName,QString("='%1'!$A$1:$%2$%3").arg(sheetName)
                         .arg(QChar(QChar('A').unicode()+query->record().count()-1)).arg(i)))
           qDebug()<<"Can not define aria name"<<ariaName;
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

void appExecuteReport(const QString &queryText, const QString &sheetName,const QString &fileName,const QString &ariaName="")
{
    DBWrapper *andonDb =cfGetObject<DBWrapper>("andonDb");
    //qApp->findChild<DBWrapper*>("andonDb");
    if(!andonDb){
        //qDebug()<<"object andonDb not found in App";
        return;
    }
    std::function<bool(QSqlQuery*, const QString&, const QString&, const QString&)> appCreateReport2 = *appCreateReport;
    andonDb->executeQuery(queryText, [appCreateReport2,sheetName,fileName,ariaName](QSqlQuery *query){
        appCreateReport2(query,sheetName,fileName,ariaName);
    });
}

static void appAddbcClients(QSqlQuery *query)
{
    BCSender*bcSender=cfGetObject<BCSender>("bcSender");
    //qApp->findChild<BCSender*>("bcSender");
    if(!bcSender){
        //qDebug()<<"object bcSender not found in App";
        return;
    }
    while(query->next())
        bcSender->addClient(query->value(0).toString());
}


#endif // MAIN_H
