#include <QApplication>

#include "qjsonrpctcpserver.h"
#include "server_rpcservice.h"
#include "sms_service.h"
#include "bcsender.h"
#include "iostreamtheard.h"
#include "dbwrapper.h"
#include "sendemail.h"
#include "message_handler.h"
#include "single_apprun.h"

//#include <QTextCodec>
//#include <QTimer>

//#include <QSharedMemory>
//#include <QSystemSemaphore>

//#include <iostream>
//#include <QTextStream>
//#include <QThread>
//#include "mainappclass.h"

//________WebUI____________
#include "serverwebthread.h"
#include "qwebchannel.h"
#include <QtWebSockets/QWebSocketServer>
#include "websocketclientwrapper.h"
#include "websockettransport.h"

//_______QtScript_______________
//#include <QtScriptTools/QJSEngineDebugger>
//#include <QtScript>

//_______Debug_______________
//#include <QFile>
//#include <stdio.h>
//#include <stdlib.h>
//#include <windows.h>

//_______SMTPEmail Lib_______________
#include "smtpclient.h"
//#include "mimepart.h"
//#include "mimehtml.h"
//#include "mimeattachment.h"
//#include "mimemessage.h"
//#include "mimetext.h"
//#include "mimeinlinefile.h"
//#include "mimefile.h"


//_______QtTelnet class_______________
#include "qttelnet.h"

//using namespace std;
#include <functional>
#include <QJSEngine>





bool Step_3_OpenDB(DBWrapper *andondb)
{
    /*****************************************
     * Start DB
     *****************************************/
    qDebug()<<"Start DB";
    //DBWrapper *andondb = new DBWrapper;
    return andondb->ConnectDB(QCoreApplication::applicationDirPath(),DATABASE_FILE);
}




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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*****************************************
     * Start Single Application
     *****************************************/

    SingleAppRun singleApp(&a);

    /*****************************************
     * Start MessageHandler
     *****************************************/
    qDebug()<<"Start MessageHandler";
    MessageHandler msgHandler;//("cp866")
//    msgHndlr.setLists(sl1,sl2,sl3);



/*
    if(isRunning(QString("<ANDON SERVER VER%1>").arg(APP_VER),QString("<ANDON SERVER VER%1 RUNNING>").arg(APP_VER)))
    {
        qDebug() << "Server is already running!";
        a.quit();
        return 0;
    }
    else
    {
        //qDebug() << "It is first Server!";
    }

*/

//    QString pid= QString::number(a.applicationPid());
//    QSharedMemory shmem;
//    if(isRunning(QString("<ANDON SERVER VER%1>").arg(APP_VER),
//                 QString("<ANDON SERVER VER%1 RUNNING>").arg(APP_VER),
//                 pid,shmem)) {
//        qDebug()<<"SERVER is already running";
//        int answer=QMessageBox::question(new QWidget, "SERVER already running!",
//                                     "Terminate concurent application?");
//        QString old_pid;
//        switch (answer) {
//            case QMessageBox::Yes:
//                QProcess * processKill;
//                shmem.lock();
//                old_pid = QString::number(a.applicationPid());
//                memcpy( (char*)shmem.data(), (char *)old_pid.toLatin1().data(),
//                        qMin( shmem.size(), old_pid.size()));
//                shmem.unlock();
//                qDebug()<<QString("taskkill /t /PID %1").arg(pid); // /f
//                processKill->startDetached(QString("taskkill /t /PID %1").arg(pid)); // /f

//                break;
//            case QMessageBox::No:
// //                a.quit();
// //                return 0;
// //                break;
//            default:
//                qDebug()<<"answer"<<answer;
//                a.quit();
//                return 0;
//                break;
//        }
//    }


    /*****************************************
     * Start QTextCodec
     *****************************************/
//    qDebug()<<"Start QTextCodec";
/*    QTextCodec *codec;
    if (QCoreApplication::applicationDirPath().toLower().contains("build")) {
        //if (QCoreApplication::applicationDirPath().toLower().contains("cp866"))
//            codec = QTextCodec::codecForName("cp866");
        //codec = QTextCodec::codecForName("iso8859-1");
        //else
            codec = QTextCodec::codecForName("cp866");
        //    codec = QTextCodec::codecForName("utf8");
        //QJSEngineDebugger * ScriptDebuger = new QJSEngineDebugger;
        //ScriptDebuger->action(QJSEngineDebugger::InterruptAction);
        //ScriptDebuger->attachTo(engine);
        //ScriptDebuger->standardWindow()->show();
    } else {
        codec = QTextCodec::codecForName("cp866");
    }
    qDebug()<<"QTextCodec" << codec->name();
    QTextCodec::setCodecForLocale(codec);
 */
//    codec = QTextCodec::codecForName("cp866");
//    codecUTF8 = QTextCodec::codecForName("UTF8");
//    QTextCodec::setCodecForLocale(codecUTF8);
//    QTextCodec::setCodecForLocale(codecCP866);
//    QTextCodec::setCodecForLocale(codecCP1251);



//    QString fileNameFormat="DebugLogServer_yyyy_MMMM.txt";
//    logFile->setFileName(QDate().currentDate().toString(fileNameFormat));
//    outStream->setDevice(logFile);
//    outStream->setCodec(codecCP1251);


//    auto timerFN = new QTimer(QAbstractEventDispatcher::instance());
//    timerFN->setTimerType(Qt::VeryCoarseTimer);
//    QObject::connect(timerFN, &QTimer::timeout, [logFile,fileNameFormat,timerFN]{
//        qDebug()<<"timerFN restart";
//        logFile->setFileName(QDate().currentDate().toString(fileNameFormat));
//        timerFN->stop();
//        timerFN->start(msecsPerDay-QTime::currentTime().msecsSinceStartOfDay()+1000);
//    });
//    timerFN->start(0);

//    //Default log escape
//    consoleContextEscapeList->append("sms_service.cpp");
//    logfileContextEscapeList->append("serverwebthread.h");
//    consoleContextEscapeList->append("serverwebthread.h");

//    qInstallMessageHandler([outStream,logFile,consoleContextEscapeList,logfileContextEscapeList]
//                           (QtMsgType Type,const QMessageLogContext &Context,const QString &Message){
//        QString contextFile = QString(Context.file).split("\\").last().split("/").last();
//        QString contextFunction = QString(Context.function).split("(").first().split(" ").last();
//        QStringList msgList;
//        msgList << QDateTime::currentDateTime().toString("d ddd HH:mm:ss.zzz");
//        if ((!contextFile.isEmpty()) && (Context.line!=0))
//            msgList << contextFile << contextFunction << QString::number(Context.line);
//        msgList << Message << "\n";
//        QString msg=msgList.join(" ");
//        if (!consoleContextEscapeList->contains(contextFile)){
//            fprintf(stdout,msg.toLocal8Bit().constData());
//            fflush(stdout);
//        }
//        if (!logfileContextEscapeList->contains(contextFile)){
//            if(logFile->open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append)) {
//                outStream->operator <<(msg.replace("\\\"","\""));
//                logFile->close();
//            }
//        }
//    });

//    qDebug()<<"____________________________________________";

    /*****************************************
     * Start DataBase
     *****************************************/

    DBWrapper *andondb = new DBWrapper;
    if(!Step_3_OpenDB(andondb)) {
        qDebug() << "Cannot connect to DB";
        a.quit();
        return 0;
    }


    /*****************************************
     * Start QtTelnet
     *****************************************/
    qDebug()<<"Start telnetClient";
    QtTelnet * telnetClient = new QtTelnet;
    telnetClient->setObjectName("telnetClient");
    telnetClient->start();

    /*****************************************
     * Start andonrpcservice
     *****************************************/
    qDebug()<<"Start andonrpcservice";
    ServerRpcService * andonrpcservice = new ServerRpcService;
    QJsonRpcTcpServer * rpcserver = new QJsonRpcTcpServer;
    rpcserver->setObjectName("JSONRPC_SERVER");

    QObject::connect(rpcserver, &QJsonRpcTcpServer::clientConnected, [=] (const QHostAddress &clientIP) {
        qDebug()<<"clientConnected"<<clientIP.toString();
        QJsonObject joClient;
        joClient.insert("STATION_IP", clientIP.toString());
        joClient.insert("EVENT_ID", QTime::currentTime().toString("HH:mm:ss.zzz"));
        joClient.insert("STATUS", "CONNECTED");
        QJsonDocument jdClient(joClient);
        andonrpcservice->StartSms(jdClient.toJson(QJsonDocument::Compact));
    });
    QObject::connect(rpcserver, &QJsonRpcTcpServer::clientDisconnected, [=] (const QHostAddress &clientIP) {
        qDebug()<<"clientDisconnected"<<clientIP.toString();
        QJsonObject joClient;
        joClient.insert("STATION_IP",clientIP.toString());
        joClient.insert("EVENT_ID",QTime::currentTime().toString("HH:mm:ss.zzz"));
        joClient.insert("STATUS", "DISCONNECTED");
        QJsonDocument jdClient(joClient);
        andonrpcservice->StartSms(jdClient.toJson(QJsonDocument::Compact));
    });
    rpcserver->addService(andonrpcservice);
    andonrpcservice->setDB(andondb);

//    QTimer *rpcTimer = new QTimer;
//    QObject::connect(rpcTimer,&QTimer::timeout,[=](){
//        if (rpcserver->listen(QHostAddress::AnyIPv4, JSONRPC_SERVER_PORT)) {
//            qDebug()<<"JSONRPC PROT OPENED"<<JSONRPC_SERVER_PORT;
//            rpcTimer->stop();
//            rpcTimer->deleteLater();
//        } else {
//            qDebug() << "rpcTimer: can't start JSONRPC service on server: " << rpcserver->errorString();
//            rpcTimer->start(2000);
//        }
//    });
//    rpcTimer->start(700);

    listenPort<QJsonRpcTcpServer>(rpcserver,JSONRPC_SERVER_PORT,3000,700);
    /*****************************************
     * Start Unicast UDP Sender
     *****************************************/
    qDebug()<<"Start Unicast UDP Sender";
    //TODO: server startup delay to DB
    ioStreamThread * iotheard = new ioStreamThread;
    Sender * sender = new Sender;

    //QJsonDocument jdocClients(QJsonDocument::fromJson(andondb->query2jsonarrays(
    QJsonDocument jdocClients(QJsonDocument::fromJson(andondb->query2json(
                                                          QString("SELECT IP_ADDRESS "
                                                                  "FROM TBL_STATIONS "
                                                                  "WHERE ENABLED='1'")
                                                          ).toUtf8()));
    //QJsonObject ClientsObject = jdocClients.object();
    QJsonArray ClientsArray = jdocClients.array();
    QJsonObject recordObject;
    //ClientsObject.remove("FieldList");
    for (int i=0;i<ClientsArray.count();++i) {
        recordObject=ClientsArray.at(i).toObject();
        sender->addClient(recordObject[recordObject.keys().at(0)].toString());
    }


    QTimer StartUpDelay;
    StartUpDelay.setInterval(2000);
    StartUpDelay.setSingleShot(true);
    StartUpDelay.start();
    QObject::connect(&StartUpDelay,&QTimer::timeout,[=](){

        iotheard->start();
        sender->run(UDP_INTERVAL,UDP_PORT);
        //TODO: stop broadcast after connect all clients
        QObject::connect(iotheard, &ioStreamThread::inputReceived, [sender] (const QString &text) {
            qDebug()<<"recived text:" << text;
            if (text.toUpper().left(5)=="RENEW" && text.length()>7) {
                QString ClientIPstr =text.toUpper().mid(6);
                qDebug()<<"ClientIPstr:" << ClientIPstr;
                if (ClientIPstr=="ALL")
                    sender->renewInterface();
                else
                    sender->renewInterface(ClientIPstr);
                //qDebug()<<"sender->renewInterface" << ClientIPstr;
            }
        });
        iotheard->start();
    });

 /*****************************************
 * Start WebUI
 *****************************************/
    qDebug()<<"Start WebUI";
    QWebSocketServer webSocketServer(QStringLiteral("QWebChannel Server"), QWebSocketServer::NonSecureMode);
    webSocketServer.setObjectName("webSocketServer");
    listenPort<QWebSocketServer>(&webSocketServer,WEBCHANEL_PORT,3000,500);

    WebSocketClientWrapper clientWrapper(&webSocketServer);
    WebuiThread *WThread = new WebuiThread;
    WThread->setObjectName("WebuiThread");
    WThread->start();
    listenPort<WebuiThread>(WThread,WUI_PORT,3000,1000);

//    qDebug()<<"Start setup the channel";
    QWebChannel channel;
    QObject::connect(&clientWrapper, &WebSocketClientWrapper::clientConnected,
                     &channel, &QWebChannel::connectTo);
    channel.registerObject(QStringLiteral("webui"), WThread);

    QObject::connect(WThread,static_cast<void (WebuiThread::*)(const QString &sql_query,
                              std::function<void(QSqlQuery *query)> functor)>(&WebuiThread::getSqlQuery),
                     andondb, static_cast<void (DBWrapper::*)(const QString &sql_query,
                              std::function<void(QSqlQuery *query)> functor)>(&DBWrapper::executeQuery));

    QObject::connect(WThread,static_cast<void (WebuiThread::*)(const QString &sql_query,const QString &query_method,
                              std::function<void(QString jsontext)> functor)>(&WebuiThread::getSqlQuery),
                     andondb, static_cast<void (DBWrapper::*)(const QString &sql_query,const QString &query_method,
                              std::function<void(QString jsontext)> functor)>(&DBWrapper::executeQuery));
//    qDebug()<<"Finish setup the channel";

//    SchedulerStruc PdpScheduler={[WThread](){
//                                  if(QDate::currentDate().dayOfWeek()<5) {
//                                      QStringList rcpnts;
//                                      if(QDate::currentDate().weekNumber() &
//            ((QTime::currentTime().hour()*60+QTime::currentTime().minute())/870 ) )
//                                           rcpnts<<"stadulskiy@faurecia.com";
//                                      else rcpnts<<"poloznov@faurecia.com";
//                                      WThread->snedReport("PDP", rcpnts);
//                                  }
//            },QList<QTime>()<<QTime::fromString("14:15:00")<<QTime::fromString("22:45:00"),0};
//    SchedulerStruc PdpScheduler2={[WThread](){
//                                      WThread->snedReport("PDP", QStringList()<<"ilya-kolesnic@yandex.ru");
//                                  },QList<FloatTimeStruc>()<<endOfShift2<<endOfShift1Even,0};

//    schedulerList<<PdpScheduler;

    /*****************************************
    * Start pdpTimer
    *****************************************/
    qDebug()<<"Start pdpTimer";
    const int msecsPerDay = 24 * 60 * 60 * 1000;

    QTimer * pdpTimer = new QTimer(QAbstractEventDispatcher::instance());
    pdpTimer->setTimerType(Qt::VeryCoarseTimer);
    pdpTimer->start(msecsPerDay-max(QTime::fromString("14:15:00").elapsed(),
                                    QTime::fromString("22:45:00").elapsed())+1000);
    qDebug()<<"pdpTimer start"<<pdpTimer->interval()/3600000.0<<"hours";
    QObject::connect(pdpTimer,&QTimer::timeout, [WThread,pdpTimer](){
        qDebug()<<"pdpTimer timeout"<<"dayOfWeek"<<QDate::currentDate().dayOfWeek();
        if(QDate::currentDate().dayOfWeek()<5) {
            QStringList rcpnts("ilya.kolesnik@faurecia.com");
            if(QDate::currentDate().weekNumber() &
             ((QTime::currentTime().hour()*60+QTime::currentTime().minute())/870))
                 rcpnts<<"evgeny.stadulsky@faurecia.com";
            else rcpnts<<"alexander.poloznov@faurecia.com";
            WThread->snedReport("Daily_PDP", rcpnts);
        }
        pdpTimer->start(msecsPerDay-max(QTime::fromString("14:15:00").elapsed(),
                                        QTime::fromString("22:45:00").elapsed())+1000);
        qDebug()<<"pdpTimer start"<<pdpTimer->interval()/3600000.0<<"hours";
    });
    //qDebug()<<"WThread->snedReport";

/*
    QObject::connect(schedulerTimer,&QTimer::timeout,[&schedulerList,schedulerTimer](){
        for(auto s : schedulerList) {
            if (!s.eventTimer){
                s.eventTimer=new QTimer;
                QObject::connect(s.eventTimer,&QTimer::timeout,s.functor);

                QDateTime nextEvent;
                QTime curTime=QTime::currentTime();
                QDate curDate=QDate::currentDate();
                QDate checkDate=curDate;
                for(int i=0;i<31;++i) {
                    for(auto s : schedulerList)
                        for(auto f : s.eventFloatTimes) {
                            if (f.dayOfMonth==checkDate.day() && (checkDate!=curDate || f.eventTime>curTime)) {
                                nextEvent=curDate;
                                nextEvent.setTime(f.eventTime);
                                s.eventTimer->start(QDateTime::msecsTo(nextEvent));
                                break;
                            }
                            if (f.eventWeekdays.contains(checkDate.dayOfWeek()) && (checkDate!=curDate || f.eventTime>curTime)) {
                                nextEvent;
                                nextEvent.setTime(f.eventTime);
                                s.eventTimer->start(QDateTime::msecsTo(nextEvent));
                                break;
                            }
                        }

                    checkDate.addDays(1)
                }
            }
        }

    });
*/

    //snedReport(const QString &report, const QStringList &emails)

//    QTimer WebuiUpdate;
//    WebuiUpdate.setInterval(10000);
//    WebuiUpdate.setSingleShot(false);
//    WebuiUpdate.start();
//    QObject::connect(&WebuiUpdate,&QTimer::timeout,[andondb,WThread](){
//        WThread->updateCurStatuses("table",QJsonDocument::fromJson(
//            andondb->query2fulljson("SELECT * FROM VIEW_CURRENT_STATUSES_RU").toUtf8()).toJson());
//        WThread->updateCurStatuses("graff",QJsonDocument::fromJson(
//            andondb->query2json("SELECT * FROM VIEW_STATUS_LOG2GRAFF").toUtf8()).toJson());
//    });

    /*****************************************
     * Start SMS Server
     *****************************************/
    qDebug()<<"Start SMS Server";
    Sms_service * sms_sender = new Sms_service;
    QObject::connect(andonrpcservice,&ServerRpcService::SendSMS, sms_sender,
             &Sms_service::sendSMSFECT,Qt::QueuedConnection);

    QObject::connect(sms_sender,&Sms_service::SmsStatusUpdate,[andondb]
                         (int SmsLogId, int SmsId, int Status){
            QString lastError;
            andondb->queryexecute(QString("EXECUTE PROCEDURE SERVER_UPDATE_SMSSTATUS(%1, %2, %3)")
                          .arg(SmsLogId).arg(SmsId).arg(Status), lastError);
    });

    QString sqlqueryres = andondb->query2json("SELECT AUX_PROPERTIES_LIST "
                                                    "FROM TBL_TCPDEVICES "
                                                    "WHERE DEVICE_TYPE='SMS Server' AND DEVICE_TYPE='SMS Server'");
    QJsonDocument jdocURL;
    if (!sqlqueryres.isEmpty()){
        jdocURL= QJsonDocument::fromJson(sqlqueryres.toUtf8());
        QJsonArray URLArray = jdocURL.array();
        QJsonObject URLObject = URLArray.at(0).toObject();
        if (URLObject.contains("AUX_PROPERTIES_LIST")){
            jdocURL = QJsonDocument::fromJson(URLObject["AUX_PROPERTIES_LIST"].toString().toUtf8());
            URLObject = jdocURL.object();
            sms_sender->setURL(URLObject["URL"].toString());
        }
    } else
        sms_sender->setURL("http://10.208.98.101:81/sendmsg?user=SMS1&passwd=smssms1&cat=1&enc=%1&to=%2&text=%3");
    //sms_sender.start();
    QThread* smsThread = new QThread;
    sms_sender->moveToThread(smsThread);
    smsThread->start();

    /*****************************************
     * Start SendEmail
     *****************************************/
    qDebug()<<"Start SmtpClient";
    //TODO From DB

    SendEmail * emailClient = new SendEmail;
    emailClient->setObjectName("emailClient");
    QObject::connect(emailClient,&SendEmail::errorMessage,[](const QString & message){
        qDebug()<<"Email errorMessage"<<message;
    });
    QObject::connect(WThread,&WebuiThread::sendReport2email,[emailClient]
                    (const QString &subject, const QString &message,
                     const QStringList &rcptStringList, QList<QBuffer*> *attachments=0){
        emailClient->sendEmail(subject, message, rcptStringList, attachments);
    });


    /*****************************************
     * Start QXlsx::Document
     *****************************************/
    /*
    qDebug()<<"Start QXlsx::Document";

    QXlsx::Document * xlsx = new QXlsx::Document("PDP Injection CW 05 (2016) TST.xlsx");
    qDebug()<<"xlsx setObjectName";
    xlsx->setObjectName("xlsx");
    */
    /*
     xlsx->selectSheet("Injection");
    int val = xlsx->read(QString("BD193")).toInt();
    qDebug()<<"xlsx val="<<val<<"variant"<<xlsx->read(QString("BD193"));
    val=val+19;
    xlsx->write(QString("BD193"),QVariant(val));
    qDebug()<<"xlsx SR_1_CNT_1409806XCF="<<val;
    //SR_1_CNT_1409806XCF
    //xlsx->write(1,2, "val");
    qDebug()<<"xlsx QBuffer";
    */
    /*
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    qDebug()<<"xlsx saveAs(&buffer)";
    xlsx->saveAs(&buffer);
    //qDebug()<<"xlsx buffer.open";
    buffer.open(QIODevice::ReadOnly);
    qDebug()<<"emailClient->sendAttachment";
    emailClient->sendAttachment(QString("new xlsx document"),
                                QStringList()<<QString("ilya.kolesnik@faurecia.com"),
                                //QStringList()<<QString("ilya-kolesnic@yandex.ru"),
                                QString("find new xlsx document in attachments"),
                                buffer.readAll(),QString("PDP Injection CW 05 (2016) TST").append(".xlsx"));
    */

    /*****************************************
     * Start Script Engine
     *****************************************/
    qDebug()<<"Start Script Engine";

    QJSEngine *engine = new QJSEngine;
    engine->globalObject().setProperty("msgHandler",engine->newQObject(&msgHandler));
    engine->globalObject().setProperty("telnetClient",engine->newQObject(telnetClient));
    engine->globalObject().setProperty("andonrpcservice",engine->newQObject(andonrpcservice));
    engine->globalObject().setProperty("sms_sender",engine->newQObject(sms_sender));
    engine->globalObject().setProperty("andondb",engine->newQObject(andondb));
    engine->globalObject().setProperty("emailClient",engine->newQObject(emailClient));
    engine->globalObject().setProperty("WThread",engine->newQObject(WThread));

    WThread->setEngine(engine);


    /*****************************************
     * Start ServerStartScript Evaluate
     *****************************************/
    qDebug()<<"Start ServerStartScript Evaluate";

    QStringList ScriptsList;
    QJsonDocument jdocScripts(QJsonDocument::fromJson(andondb->query2json(
                                                          QString("SELECT SCRIPT_TEXT "
                                                          "FROM TBL_SCRIPTS WHERE SCRIPT_NAME='ScriptServerStart'")).toUtf8()));
    QJsonArray tableArray = jdocScripts.array();
    if (!tableArray.isEmpty())
        recordObject=tableArray.at(0).toObject();


/*
    if (recordObject.keys().contains("SCRIPT_ORDER") && recordObject.keys().contains("SCRIPT_TEXT")) {
        QList<QJsonObject> scriptObjectList;
        for (int i=0;i<tableArray.count();++i)
            scriptObjectList.append(tableArray.at(i).toObject());
        QByteArray sortfield = "SCRIPT_ORDER";
        std::sort(scriptObjectList.begin(), scriptObjectList.end(),[sortfield](const QJsonObject a, const QJsonObject b ){
            return a[sortfield].toInt() < b[sortfield].toInt();
        });
        for (int i=0;i<scriptObjectList.count();++i){
            QString ScriptText = scriptObjectList.at(i)["SCRIPT_TEXT"].toString().replace("`","'");
            QScriptSyntaxCheckResult result = engine->checkSyntax(ScriptText);
            if (result.state()==QScriptSyntaxCheckResult::Valid) {
                ScriptsList.append(ScriptText);
            }
            else {
                qDebug()<<"Syntax error in script "<<"("<<result.errorLineNumber()<<
                          ","<<result.errorColumnNumber()<<"):";
                qDebug()<<ScriptText<<"\nMessage:"<<result.errorMessage();
            }
        }
                for (int i = 0; i<ScriptsList.count(); ++i) {
                    //qDebug()<<"server_start"<<ScriptsList.at(i);
                    engine->evaluate(ScriptsList.at(i));
                }
    }
*/
    QList<QJsonObject> scriptObjectList;
    for (int i=0;i<tableArray.count();++i)
        scriptObjectList.append(tableArray.at(i).toObject());
    for (int i=0;i<scriptObjectList.count();++i){
        QString ScriptText = scriptObjectList.at(i)["SCRIPT_TEXT"].toString().replace("`","'");
//        QScriptSyntaxCheckResult result = engine->checkSyntax(ScriptText);
//        if (result.state()==QScriptSyntaxCheckResult::Valid) {
//            ScriptsList.append(ScriptText);
//        }
//        else {
//            qDebug()<<"Syntax error in script "<<"("<<result.errorLineNumber()<<
//                      ","<<result.errorColumnNumber()<<"):";
//            qDebug()<<ScriptText<<"\nMessage:"<<result.errorMessage();
//        }
    }
            for (int i = 0; i<ScriptsList.count(); ++i) {
                //qDebug()<<"server_start"<<ScriptsList.at(i);
                engine->evaluate(ScriptsList.at(i));
            }

//    WThread->snedReport("Daily_PDP", QStringList()<<"ilya.kolesnik@faurecia.com");
    qDebug()<<"main finish";
    return a.exec();
}
