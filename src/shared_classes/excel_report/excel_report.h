//*******************************************************************************
#ifndef EXCEL_REPORT_H
#define EXCEL_REPORT_H

#include <QObject>
#include "xlsxdocument.h"
QTXLSX_USE_NAMESPACE
//#include <QTime>
#include <QDateTime>

//*******************************************************************************
class ExcelReport: public QObject
{
    Q_OBJECT
public:
    ExcelReport(QObject * parent=0);
    ~ExcelReport();

public slots:
    bool queryText2File(const QString &queryText, const QString &sheetName,
                        const QString &fileName, const QString &ariaName="");
//    bool queryText2Ftp(const QString &queryText, const QString &sheetName,
//                        const QString &fileName, const QString &ariaName="");
    bool queryText2Email(const QString & queryText, const QString &subject,
                         const QStringList &rcptStringList, const QString &message="",
                         QString sheetName="", QString fileName="");
    void addFileReport(QTime baseTime, char unit, int interval,
                       const QString &queryText, const QString &sheetName,
                       const QString &fileName, const QString &ariaName="");
    void addEmailReport(QTime baseTime, char unit, int interval,
                        const QString & queryText, const QString &subject,
                        const QStringList &rcptStringList, const QString &message="",
                        const QString &sheetName="", const QString &fileName="");
private:
    bool queryText2Document(const QString & queryText, Document * xlsx,
                            const QString &sheetName, const QString &ariaName="");
    Document * newDocument(const QString &sheetName = "report");
    qint64 calcNextInterval(QTime baseTime, char unit, int interval);

};
#endif //EXCEL_REPORT_H
