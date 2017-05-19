#include "excel_report.h"
#include <QtCore/QDebug>
#include "functional"
#include "common_functions.h"

#include "dbwrapper.h"
#include "sendemail.h"
//#include "qftp.h"

#include <QBuffer>
#include <QTimer>

ExcelReport::ExcelReport(QObject *parent) :
    QObject(parent)
{

}

ExcelReport::~ExcelReport()
{

}

bool ExcelReport::queryText2Document(const QString & queryText, Document *xlsx,
                                     const QString &sheetName, const QString &ariaName)
{
    DBWrapper *andonDb =cfGetObject<DBWrapper>("andonDb");
    if(!andonDb)
        return false;
    QSqlQuery *query = andonDb->sql2Query(queryText);
    if(!query)
        return false;
    QTextCodec *codec = QTextCodec::codecForName("iso8859-1");
    for(int j=0; j < query->record().count(); j++)
        xlsx->write(1,j+1,QString(codec->fromUnicode(query->record().fieldName(j))));
    int i=1;
    while(query->next()) {
        i++;
        for(int j=0; j < query->record().count(); j++)
            xlsx->write(i,j+1,query->value(j));
    }
    if(!ariaName.isEmpty())
        if(!xlsx->defineName(ariaName,QString("='%1'!$A$1:$%2$%3").arg(sheetName)
                         .arg(QChar(QChar('A').unicode()+query->record().count()-1)).arg(i)))
           qDebug()<<"Can not define aria name"<<ariaName;
    return true;
}

bool ExcelReport::queryText2Email(const QString & queryText, const QString &subject,
                                  const QStringList &rcptStringList, const QString &message,
                                  QString sheetName, QString fileName)
{
    if(sheetName.isEmpty())
        sheetName=subject;
    if(fileName.isEmpty())
        fileName=subject;
    Document * xlsx = newDocument(sheetName);
    if(!queryText2Document(queryText,xlsx,sheetName))
        return false;
    QBuffer *buffer = new QBuffer(new QByteArray);
    xlsx->saveAs(buffer);
    buffer->setProperty("FILE_NAME",fileName);
    SendEmail *emailClient =cfGetObject<SendEmail>("emailClient");
    if(!emailClient)
        return false;
    return emailClient->sendEmail(subject, message, rcptStringList, QList<QBuffer*>()<<buffer);
}

bool ExcelReport::queryText2File(const QString &queryText, const QString &sheetName,
                                 const QString &fileName,const QString &ariaName)
{
    Document * xlsx = newDocument(sheetName);
    if(!queryText2Document(queryText,xlsx,sheetName,ariaName))
        return false;
    if(xlsx->saveAs(fileName)){
        qDebug()<<fileName<<".xlsx save OK";
        return true;
    }
    else
        qDebug()<<fileName<<".xlsx not saved";
    return false;
}

Document * ExcelReport::newDocument(const QString &sheetName)
{
    Document *xlsx= new Document;
    if(xlsx->selectSheet(sheetName))
        xlsx->deleteSheet(sheetName);
    xlsx->addSheet(sheetName);
}

void ExcelReport::addEmailReport(QTime baseTime, char unit, int interval,
                    const QString & queryText, const QString &subject,
                    const QStringList &rcptStringList, const QString &message,
                    const QString &sheetName,const QString &fileName)
{
    QTimer * t = new QTimer;
    QObject::connect(t,&QTimer::timeout,[=](){
        queryText2Email(queryText,subject,rcptStringList,message,sheetName,fileName);
        t->start(calcNextInterval(baseTime, unit, interval));
    });
    t->start(calcNextInterval(baseTime, unit, interval));
}

void ExcelReport::addFileReport(QTime baseTime, char unit, int interval,
                   const QString &queryText, const QString &sheetName,
                   const QString &fileName,const QString &ariaName)
{
    QTimer * t = new QTimer;
    QObject::connect(t,&QTimer::timeout,[=](){
        queryText2File(queryText,sheetName,fileName,ariaName);
        t->start(calcNextInterval(baseTime, unit, interval));
    });
    t->start(calcNextInterval(baseTime, unit, interval));
}

qint64 ExcelReport::calcNextInterval(QTime baseTime, char unit, int interval)
{
    QDateTime cdt(QDateTime::currentDateTime());
    QDateTime nextTime(cdt);
    if(unit==(uint)"M")
        nextTime.addMonths(interval);
    if(unit==(uint)"d")
        nextTime.addDays(interval);
    if(unit==(uint)"s")
        nextTime.addSecs(interval);
    nextTime.setTime(baseTime);
    qint64 result = cdt.msecsTo(nextTime);
    if(result<0)
        result = DEF_REPORT_INTERVAL;
    return result;
}

//bool ExcelReport::queryText2Ftp(const QString &queryText, const QString &sheetName,
//                                 const QString &fileName,const QString &ariaName)
//{
//    QFtp *ftp = new QFtp(jsonRow["TCPDEVICE_IP"].toString(),jsonRow["PORT"].toInt(),
//            jsonRow["LOGIN"].toString(),jsonRow["PASS"].toString());
//    QBuffer *buffer=new QBuffer;
//    QTimer *fileTimer = new QTimer;
//    fileTimer->setTimerType(Qt::VeryCoarseTimer);
//    QObject::connect(ftp, &QFtp::commandFinished,[ftp,buffer,serverRpc](int command,bool res){
//        if(command==buffer->property("command").toInt()){
//            int taskId=buffer->property("task").toInt();
//            buffer->buffer().clear();
//            buffer->close();
//            if(res){
//                taskId=-taskId;
//                qDebug() << "Ftp: file not wirted";
//            }
//            serverRpc->Query2Json(QString("SELECT PART_REFERENCE, "
//                                          "PART_COUNT FROM PRODUCTION_DECLARATING(%1)").arg(taskId),
//                                  [](QVariant resp){
////                                qDebug() << "PRODUCTION_DECLARATING finish" << resp.toString().size();
////                                QJsonArray array = QJsonDocument::fromJson(resp.toString().toUtf8()).array();
////                                if(!array.isEmpty()) {
////                                    QJsonObject jsonObj0=array.at(0).toObject();
////                                    if(jsonObj0.contains("PART_REFERENCE") && jsonObj0.contains("PART_COUNT")){
////                                        qDebug() << "Ftp: file writed";
////                                        for (auto object:array) {
////                                            QJsonObject jsonObj=object.toObject();
////                                            qDebug() << jsonObj["PART_REFERENCE"].toString()
////                                                    << jsonObj["PART_COUNT"].toInt();
////                                        }
////                                    }
////                                }
//            });
//        }
//    });
//    QObject::connect(fileTimer,&QTimer::timeout,
//                     [serverRpc,ftp,buffer,fileTimer](){
//        QDateTime cdt = QDateTime::currentDateTime();
//        //int interval = qMin(FTP_INTERVAL+120,qMax(FTP_INTERVAL-120, cdt.msecsTo(QDateTime(cdt.addDays(cdt.time().hour()==23?1:0), QTime(cdt.time().hour()+1,58)))));
//        QDateTime ndt = QDateTime::currentDateTime().addSecs(3600);
//        int interval = cdt.msecsTo(QDateTime(ndt.date(),QTime(ndt.time().hour(),58)));
//        //qDebug() << "fileTimer->start(" << interval << ");";
//        fileTimer->start(interval);
//        serverRpc->Query2Json("SELECT ID_TASK, PART_REFERENCE, PART_COUNT, MANUFACTURE_DATE "
//                              "FROM PRODUCTION_DECLARATING",
//                              [ftp,buffer](QVariant resp){
//            qDebug() << "PRODUCTION_DECLARATING start" << resp.toString().size();
//            QJsonArray array = QJsonDocument::fromJson(resp.toString().toUtf8()).array();
//            qDebug() << "PRODUCTION_DECLARATING lambda 1";
//            if(!array.isEmpty()) {
//                qDebug() << "PRODUCTION_DECLARATING lambda 2";
//                QJsonObject jsonObj0=array.at(0).toObject();
//                qDebug() << "PRODUCTION_DECLARATING lambda 3";
//                if(jsonObj0.contains("PART_REFERENCE") && jsonObj0.contains("ID_TASK")
//                        && jsonObj0.contains("PART_COUNT")) {
//                    int taskId=jsonObj0["ID_TASK"].toInt();
//                    qDebug() << "PRODUCTION_DECLARATING lambda 4";
//                    buffer->setProperty("task", taskId);
//                    buffer->open(QBuffer::ReadWrite);
//                    for (auto object = array.begin(); object != array.end(); object++) {
//                        QJsonObject jsonObj=object->toObject();
//                        buffer->write(jsonObj["PART_REFERENCE"].toString().toLatin1());
//                        buffer->write("\t");
//                        buffer->write(QString::number(jsonObj["PART_COUNT"].toInt()).toLatin1());
//                        //buffer->write("\t");
//                        //buffer->write(jsonObj["MANUFACTURE_DATE"].toString().toLatin1());
//                        buffer->write("\r\n");
//                    }
//                    qDebug() << "PRODUCTION_DECLARATING lambda 5";
//                    QTimer::singleShot(0,[ftp,buffer,taskId](){
//                        qDebug() << "PRODUCTION_DECLARATING lambda 6";
//                        buffer->setProperty("command",ftp->putBuf(buffer,
//                                                                  QString("Decl_%1.txt").arg(QDateTime().currentDateTime().toString("ddMMyy_hh_mm")), QFtp::Binary,taskId));
//                        qDebug() << "ftp putBuf lambda 7";
//                    });
//                }
//            }
//        });
//    });
//}
