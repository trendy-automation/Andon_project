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

//________WebUI____________
#include "serverwebthread.h"
#include "qwebchannel.h"
#include <QtWebSockets/QWebSocketServer>
#include "websocketclientwrapper.h"
#include "websockettransport.h"

//_______QtTelnet class_______________
//#include "qttelnet.h"

#include <functional>
#include <QJSEngine>
#include "main.h"

#include "watchdog.h"

#include <QMessageBox>
#include <QVariant>

#include "excel_report.h"

Q_DECLARE_METATYPE(QJsonRpcTcpServer*)

int main(int argc, char *argv[])
{
    //qRegisterMetaType<QJsonRpcTcpServer*>("QJsonRpcTcpServer");
    QApplication a(argc, argv);
    /*****************************************
     * Start Watchdog
     *****************************************/
    QStringList args = a.arguments();
    Watchdog *watchdog = new Watchdog/*(&a)*/;
    watchdog->setObjectName("watchdog");
    if(args.contains(APP_OPTION_WATHCDOG)){
        if(!watchdog->listen(JSONRPC_SERVER_WATCHDOG_PORT,QString(JSONRPC_WATCHDOG_SERVICENAME).append(".isAlive")))
            //appClientDisconnected();
            qDebug() << "Watchdog application cannot run!";
        return a.exec();
    }
//    else
//        QTimer::singleShot(10000,[](){
//            qDebug() << "Test crash application";
//            QObject*null;
//            null->setObjectName("crash");
//        });
    /*****************************************
     * Start MessageHandler
     *****************************************/
    MessageHandler msgHandler;//("cp866")
    /*****************************************
     * Start Single Application
     *****************************************/
    qDebug()<<"Start singleApp";
    SingleAppRun singleApp(args.contains(APP_OPTION_FORCE),&a);
    if(singleApp.isToQuit()){
        a.quit();
        return 0;
    }

    /*****************************************
     * Start DataBase
     *****************************************/
    DBWrapper *andonDb = new DBWrapper(&a);
    andonDb->setObjectName("andonDb");
    if(!andonDb->ConnectDB(qApp->applicationDirPath(),DB_DATABASE_FILE)){
        //qDebug()<<"ConnectDB failed";
        QMessageBox::information(new QWidget,"Andon server failed","Can not connect to DB");
        a.quit();
        return 0;
    }
    QObject::connect(andonDb,&DBWrapper::dbError,watchdog,&Watchdog::rebootPC);
    /*****************************************
     * Start QtTelnet
     *****************************************/
    //    qDebug()<<"Start telnetClient";
    //    QtTelnet * telnetClient = new QtTelnet;
    //    telnetClient->setObjectName("telnetClient");
    //    telnetClient->start();
    /*****************************************
     * Start andonRpcService
     *****************************************/
    qDebug()<<"Start andonRpcService";
    ServerRpcService * andonRpcService = new ServerRpcService;//(&a);
    andonRpcService->setObjectName("andonRpcService");
    QJsonRpcTcpServer * rpcServer = new QJsonRpcTcpServer;//(&a);
    a.setProperty("rpcServer", QVariant::fromValue<QJsonRpcTcpServer *>(rpcServer));
    rpcServer->setObjectName("rpcServer");
    rpcServer->addService(andonRpcService);
    andonRpcService->setDB(andonDb);
    QObject::connect(rpcServer, &QJsonRpcTcpServer::clientConnected, rpcServer, appClientConnected);
    QObject::connect(rpcServer, &QJsonRpcTcpServer::clientDisconnected, rpcServer, appClientDisconnected);
    cfListenPort<QJsonRpcTcpServer>(rpcServer,JSONRPC_SERVER_PORT,3000,700);
    /*****************************************
     * Start Unicast UDP Sender
     *****************************************/
    qDebug()<<"Start Unicast UDP Sender";
    //TODO: server startup delay to DB
    ioStreamThread * iotheard = new ioStreamThread;
    iotheard->setObjectName("iotheard");
    BCSender * bcSender = new BCSender(UDP_INTERVAL,UDP_PORT,&a);
    bcSender->setObjectName("bcSender");
    andonDb->executeQuery("SELECT IP_ADDRESS FROM TBL_STATIONS WHERE ENABLED='1'",appAddbcClients);
    QObject::connect(iotheard, &ioStreamThread::inputReceived, appParseInput);
    QObject::connect(iotheard, &ioStreamThread::started, bcSender, &BCSender::run);
    QTimer::singleShot(2000,iotheard,&ioStreamThread::startThread);
    /*****************************************
     * Start WebUI
     *****************************************/
    qDebug()<<"Start WebUI";
    QWebSocketServer webSocketServer(QStringLiteral("QWebChannel Server"), QWebSocketServer::NonSecureMode,&a);
    webSocketServer.setObjectName("webSocketServer");
    cfListenPort<QWebSocketServer>(&webSocketServer,WEBCHANNEL_PORT,3000,500);

    WebSocketClientWrapper clientWrapper(&webSocketServer);
    WebuiThread *WThread = new WebuiThread;
    WThread->setObjectName("WebuiThread");
    WThread->start();
    cfListenPort<WebuiThread>(WThread,WUI_PORT,3000,1000);

    //    qDebug()<<"Start setup the channel";
    QWebChannel channel(&a);
    QObject::connect(&clientWrapper, &WebSocketClientWrapper::clientConnected,
                     &channel, &QWebChannel::connectTo);
    channel.registerObject(QStringLiteral("serverWeb"), WThread);
    channel.registerObject(QStringLiteral("db"), andonDb);
    QObject::connect(WThread,static_cast<void (WebuiThread::*)(const QString &sql_query,
                                                               std::function<void(QSqlQuery *query)> functor)>(&WebuiThread::getSqlQuery),
                     andonDb, static_cast<void (DBWrapper::*)(const QString &sql_query,
                                                              std::function<void(QSqlQuery *query)> functor)>(&DBWrapper::executeQuery));

    QObject::connect(WThread,static_cast<void (WebuiThread::*)(const QString &sql_query,const QString &query_method,
                                                               std::function<void(QString jsontext)> functor)>(&WebuiThread::getSqlQuery),
                     andonDb, static_cast<void (DBWrapper::*)(const QString &sql_query,const QString &query_method,
                                                              std::function<void(QString jsontext)> functor)>(&DBWrapper::executeQuery));
    /*****************************************
    * Start reportTimer
    *****************************************/
    //    appExecuteReport(QString("SELECT * FROM REPORT_BREAKDOWNS('%1', '%2')")
    //                     .arg(QDate(QDate::currentDate().year(),QDate::currentDate().month(),1).toString("dd.MM.yyyy"))
    //                     .arg(QDate(QDate::currentDate().year(),(QDate::currentDate().month()+1)%12,1).toString("dd.MM.yyyy")),
    //                     QDate::currentDate().toString("Простои"),
    //        QString("P:\\!Common Documents\\Andon_reports\\Простои за прошлый месяц"),
    //        "brakedowns");

    //    appExecuteReport("SELECT * FROM PRODUCTION_DECLARATION_HISTORY", "AutoDecl",
    //        QString("P:\\!Common Documents\\AutomaticDeclarating\\AutoDecl_export"),"AutoDecl_aria");

    qDebug()<<"Start reportTimer";

    ExcelReport *excelReport=new ExcelReport(&a);
//    excelReport->addEmailReport();
//    excelReport->addFileReport();

    excelReport->queryText2File("SELECT * FROM REPORT_MONTH_DECLARATION", "AutoDecl",
                     QString("P:\\!Common Documents\\AutomaticDeclarating\\AutoDecl_export.xlsx")
                     //.arg(QDate::currentDate().toString("MMMM_yyyy"))
                     ,"AutoDecl_aria");
    excelReport->queryText2File("SELECT * FROM MNT_MOLD_REPORT", "Andon_cycle_counter",
                     "P:\\Maintenance\\Обслуживание пресс-форм\\Andon_cycle_counter.xlsx");

    const int msecsPerDay = 24 * 60 * 60 * 1000;
    QTimer * reportTimer = new QTimer(QAbstractEventDispatcher::instance());
    reportTimer->setTimerType(Qt::VeryCoarseTimer);
    QDateTime cdt = QDateTime::currentDateTime();
    reportTimer->start(qMax((qint64)10000, cdt.msecsTo(QDateTime(cdt.date(),QTime(23,50)))));
    qDebug()<<"reportTimer start"<<reportTimer->interval()/3600000.0<<"hours";
    QObject::connect(reportTimer,&QTimer::timeout, [excelReport,reportTimer,msecsPerDay,andonDb](){
        //qDebug()<<"reportTimer timeout"<<"dayOfWeek"<<QDate::currentDate().dayOfWeek();
        excelReport->queryText2File("SELECT * FROM REPORT_MONTH_DECLARATION", "AutoDecl",
                         QString("P:\\!Common Documents\\AutomaticDeclarating\\AutoDecl_%1.xlsx")
                         .arg(QDate::currentDate().toString("MMMM_yyyy")),"AutoDecl_aria");
        excelReport->queryText2File("SELECT * FROM MNT_MOLD_REPORT", "Andon_cycle_counter",
                         "P:\\Maintenance\\Обслуживание пресс-форм\\Andon_cycle_counter.xlsx");
        if(QDate::currentDate().daysInMonth()==QDate::currentDate().day())
            excelReport->queryText2File(QString("SELECT * FROM REPORT_BREAKDOWNS('%1', '%2')")
                             .arg(QDate(QDate::currentDate().year(),QDate::currentDate().month(),1).toString("dd.MM.yyyy"))
                             .arg(QDate(QDate::currentDate().year(),(QDate::currentDate().month()+1)%12,1).toString("dd.MM.yyyy")),
                             QDate::currentDate().toString("Простои"),
                             QString("P:\\!Common Documents\\Andon_reports\\Простои за прошлый месяц.xlsx"),
                             "brakedowns");
        if(QDate::currentDate().dayOfWeek()<6)
            andonDb->executeQuery("SELECT LIST(EMAIL) FROM TBL_STAFF WHERE EMAIL_REPORTING=1",
                                  [excelReport](QSqlQuery *query){
                if(query->next()){
                    QStringList rcpnts=query->value(0).toString().split(',');
                    if(!rcpnts.isEmpty()) //??? newer heppend
                        excelReport->queryText2Email("SELECT * REPORT_BREAKDOWNS",
                                                     QString("Простои производства %1").arg(QDate::currentDate().toString("ddd d MMMM")),rcpnts,"",
                                                     QString("Отчёт по простоям %1").arg(QDate::currentDate().toString("ddd d MMMM")),
                                                     QString("REPORT_BREAKDOWNS_%1.xlsx").arg(QDate::currentDate().toString("dd_MM_yyyy")));
                }
            });
        //reportTimer->stop();
        QDateTime cdt = QDateTime::currentDateTime();
        reportTimer->start(cdt.msecsTo(QDateTime(cdt.addDays(1).date(),QTime(23,50))));
        //reportTimer->start(msecsPerDay-qMax(QTime(23,50).elapsed(),
        //                                    QTime(23,50).elapsed())+1000);
        qDebug()<<"reportTimer start"<<reportTimer->interval()/3600000.0<<"hours";
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

    /*
    snedReport(const QString &report, const QStringList &emails)

    QTimer WebuiUpdate;
    WebuiUpdate.setInterval(10000);
    WebuiUpdate.setSingleShot(false);
    WebuiUpdate.start();
    QObject::connect(&WebuiUpdate,&QTimer::timeout,[andonDb,WThread](){
        WThread->updateCurStatuses("table",QJsonDocument::fromJson(
            andonDb->query2fulljson("SELECT * FROM VIEW_CURRENT_STATUSES_RU").toUtf8()).toJson());
        WThread->updateCurStatuses("graff",QJsonDocument::fromJson(
            andonDb->query2json("SELECT * FROM VIEW_STATUS_LOG2GRAFF").toUtf8()).toJson());
    });
*/
    /*****************************************
     * Start SMS Server
     *****************************************/
    qDebug()<<"Start SMS Server";
    Sms_service * sms_sender = new Sms_service;
    QObject::connect(andonRpcService,&ServerRpcService::SendSMS, sms_sender,
                     &Sms_service::sendSMSFECT,Qt::QueuedConnection);
    QObject::connect(sms_sender,&Sms_service::SmsStatusUpdate,[andonDb]
                     (int SmsLogId, int SmsId, int Status){
        andonDb->executeProc(QString("EXECUTE PROCEDURE SERVER_UPDATE_SMSSTATUS(%1, %2, %3)")
                             .arg(SmsLogId).arg(SmsId).arg(Status));
    });
    QString sqlqueryres = andonDb->query2json("SELECT AUX_PROPERTIES_LIST "
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
//    QObject::connect(WThread,&WebuiThread::sendReport2email,[emailClient]
//                     (const QString &subject, const QString &message,
//                      const QStringList &rcptStringList, QList<QBuffer*> attachments=QList<QBuffer*>()){
//        emailClient->sendEmail(subject, message, rcptStringList, attachments);
//    });


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
    //    engine->globalObject().setProperty("telnetClient",engine->newQObject(telnetClient));
    engine->globalObject().setProperty("andonRpcService",engine->newQObject(andonRpcService));
    engine->globalObject().setProperty("sms_sender",engine->newQObject(sms_sender));
    engine->globalObject().setProperty("andonDb",engine->newQObject(andonDb));
    engine->globalObject().setProperty("emailClient",engine->newQObject(emailClient));
    engine->globalObject().setProperty("WThread",engine->newQObject(WThread));

    WThread->setEngine(engine);


    /*****************************************
     * Start ServerStartScript Evaluate
     *****************************************/
    qDebug()<<"Start ServerStartScript Evaluate";

    QStringList ScriptsList;
    QJsonDocument jdocScripts(QJsonDocument::fromJson(andonDb->query2json(
                                                          QString("SELECT SCRIPT_TEXT "
                                                                  "FROM TBL_SCRIPTS WHERE SCRIPT_NAME='ScriptServerStart'")).toUtf8()));
    QJsonArray tableArray = jdocScripts.array();
    QJsonObject recordObject;
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

    /*****************************************
     * Start Watchdog
     *****************************************/
    qDebug()<<"Start Watchdog";
    watchdog->startRpcServer(JSONRPC_SERVER_WATCHDOG_PORT);
    QObject::connect(watchdog,&Watchdog::processRestart,[andonRpcService](){
        QJsonObject joClient;
        joClient.insert("STATION_IP",QHostAddress(QHostAddress::LocalHost).toString());
        joClient.insert("EVENT_ID",QTime::currentTime().toString("HH:mm:ss.zzz"));
        joClient.insert("STATUS", "DISCONNECTED");
        joClient.insert("USER_COMMENT", "Server restart!");
        QJsonDocument jdClient(joClient);
        andonRpcService->StartSms(jdClient.toJson(QJsonDocument::Compact));
    });
    if(QTime::currentTime().hour()==16){
        QJsonObject joClient;
        joClient.insert("STATION_IP",QHostAddress(QHostAddress::LocalHost).toString());
        joClient.insert("EVENT_ID",QTime::currentTime().toString("HH:mm:ss.zzz"));
        joClient.insert("STATUS", "DISCONNECTED");
        joClient.insert("USER_COMMENT", "Server is running!");
        QJsonDocument jdClient(joClient);
        andonRpcService->StartSms(jdClient.toJson(QJsonDocument::Compact));
        qDebug()<<"Server running SMS";
    }
    qDebug()<<"main finish";
    return a.exec();
}
