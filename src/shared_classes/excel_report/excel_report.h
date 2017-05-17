//*******************************************************************************
#ifndef EXCEL_REPORT_H
#define EXCEL_REPORT_H

#include <QObject>
#include "xlsxdocument.h"
QTXLSX_USE_NAMESPACE
#include "dbwrapper.h"
//#include <QSqlQuery>


//*******************************************************************************
class ExcelReport: public QObject
{
    Q_OBJECT
public:
    ExcelReport(QObject * parent=0);
    ~ExcelReport();

public slots:
    void executeReport(const QString &queryText, const QString &sheetName,const QString &fileName,const QString &ariaName="");

private:
    bool createReport(QSqlQuery *query, const QString &sheetName,QString fileName,const QString &ariaName="");


};
#endif //EXCEL_REPORT_H
