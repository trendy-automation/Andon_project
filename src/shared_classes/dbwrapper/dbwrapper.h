#ifndef DBWRAPPER_H
#define DBWRAPPER_H

#include <QObject>
#include <QtSql>
#include <QSqlError>
#include <QDateTime>
#include <QTimer>
#include <QJSEngine>


//#include "ibpp.h"
using namespace std;
#include <functional>

struct queryItem{
    QSqlQuery* query;
    QString jsonResult;
    QString method;
    QDateTime time;
    //bool success;
};

class DBWrapper : public QObject
{
    Q_OBJECT
public:
    explicit DBWrapper(QObject *parent = 0);
    ~DBWrapper();

signals:
    void DBConnected();
    void resultQuery(const QString &sql_query, const QString &jsontext);
    void sendText(const QString &sql_query,const QString &query_method);

public slots:
    QString query2fulljson(const QString & sqlquery);
    QString query2jsonstrlist(const QString & sqlquery);
    QString query2json(const QString & sqlquery);
    bool ConnectDB(const QString &DB_Path,const QString &DB_Name);
    QSqlQuery*queryexecute(const QString &queryText, QString &lastError);
    void executeQuery(const QString & sqlquery, const QString &query_method,
                      std::function<void(QString jsontext)> functor=[] (QString jsontext) {});
    void executeQuery(const QString & sqlquery,
                      std::function<void(QSqlQuery *query)> functor=[] (QSqlQuery *query) {});
    void executeProc(const QString & sqlquery);
    bool getDbState();
    void receiveText(const QString &query, const QString &query_method);
    void snedReport(const QString &report, const QStringList &emails);

private:
    QJSEngine *engine;
    QMap<QString,queryItem> queries;
    QString getQueryLastError(QSqlQuery *query);
    QString str2Json(const QString & name, const QString &val);
    QSqlDatabase DB;

};

#endif // DBWRAPPER_H
