#ifndef MAIN_H
#define MAIN_H

//#include <QFile>

//_______Excel Lib_______________
#include "xlsxdocument.h"

#include <QSqlQuery>

bool createReport(QSqlQuery *query, const QString &reportName, const QString &fileName)
{
    QXlsx::Document * xlsx;
    xlsx = new QXlsx::Document(QString(fileName).append(".xlsx"));
    xlsx->addSheet(reportName);
    int i=1;
    QTextCodec *codec = QTextCodec::codecForName("iso8859-1");
    for(int j=0; j < query->record().count(); j++)
        xlsx->write(i,j+1,QString(codec->fromUnicode(query->record().fieldName(j))));
    while(query->next()) {
        i++;
        for(int j=0; j < query->record().count(); j++)
            xlsx->write(i,j+1,query->value(j));
    }
    if(i>1)
        xlsx->save();
    else
        xlsx->deleteLater(); //Is need?
}

#endif // MAIN_H
