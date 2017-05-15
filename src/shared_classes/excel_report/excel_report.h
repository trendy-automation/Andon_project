//*******************************************************************************
#ifndef EXCEL_REPORT_H
#define EXCEL_REPORT_H

#include <QObject>

//*******************************************************************************
class ExcelReport: public QObject
{
    Q_OBJECT
public:
    ExcelReport(QObject * parent=0);
    ~ExcelReport();
};
#endif //EXCEL_REPORT_H
