#ifndef MAIN_H
#define MAIN_H

//#include <QFile>

//_______Excel Lib_______________
#include "xlsxdocument.h"

#include <QSqlQuery>
#include <QDebug>

bool createReport(QSqlQuery *query, const QString &reportName, const QString &fileName)
{
    QXlsx::Document * xlsx= new QXlsx::Document(QString(fileName).append(".xlsx"));
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
        if(xlsx->save())
            qDebug()<<fileName<<"save OK";
        else
            qDebug()<<fileName<<"not saved";
//    }
//    else
//        xlsx->deleteLater(); //Is need?
}

#endif // MAIN_H
