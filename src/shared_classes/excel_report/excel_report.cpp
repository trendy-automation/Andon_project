#include "excel_report.h"
#include <QtCore/QDebug>
#include "functional"
#include "common_functions.h"


ExcelReport::ExcelReport(QObject * parent) :
    QObject(parent)
{

}

ExcelReport::~ExcelReport()
{

}

bool ExcelReport::createReport(QSqlQuery *query, const QString &sheetName,QString fileName,const QString &ariaName)
{
//    Document * xlsx= new Document(fileName);
//    fileName = fileName.append(".xlsx");
//    if(xlsx->selectSheet(sheetName))
//        xlsx->deleteSheet(sheetName);
//    xlsx->addSheet(sheetName);
//    int i=1;
//    QTextCodec *codec = QTextCodec::codecForName("iso8859-1");
//    for(int j=0; j < query->record().count(); j++)
//        xlsx->write(i,j+1,QString(codec->fromUnicode(query->record().fieldName(j))));
//    while(query->next()) {
//        i++;
//        for(int j=0; j < query->record().count(); j++)
//            xlsx->write(i,j+1,query->value(j));
//    }
//    if(!ariaName.isEmpty())
//        if(!xlsx->defineName(ariaName,QString("='%1'!$A$1:$%2$%3").arg(sheetName)
//                         .arg(QChar(QChar('A').unicode()+query->record().count()-1)).arg(i)))
//           qDebug()<<"Can not define aria name"<<ariaName;
//        if(xlsx->save()){
//            qDebug()<<fileName<<"save OK";
//            return true;
//        }
//        else
//            qDebug()<<fileName<<"not saved";
    return false;
}

void ExcelReport::executeReport(const QString &queryText, const QString &sheetName,const QString &fileName,const QString &ariaName)
{
    DBWrapper *andonDb =cfGetObject<DBWrapper>("andonDb");
    //qApp->findChild<DBWrapper*>("andonDb");
    if(!andonDb){
        //qDebug()<<"object andonDb not found in App";
        return;
    }
//    std::function<bool(QSqlQuery*, const QString&, const QString&, const QString&)> createReport2 = *createReport;
//    andonDb->executeQuery(queryText, [createReport2,sheetName,fileName,ariaName](QSqlQuery *query){
//        createReport2(query,sheetName,fileName,ariaName);
//    });
    andonDb->executeQuery(queryText, [this,sheetName,fileName,ariaName](QSqlQuery *query){
        createReport(query,sheetName,fileName,ariaName);
    });
}
