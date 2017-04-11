#ifndef DBWRAPPER_H
#define DBWRAPPER_H

#include <QObject>
#include <QtSql>
//#include <QSqlError>
#include <QDateTime>
#include <QTimer>
#include <QJSEngine>




//#include "ibpp.h"
using namespace std;
#include <functional>

struct queryStruc{
    QSqlQuery* p_query;
    QString s_sql_query;
    QString s_method;
    QString s_result;
    QString s_error;
    int i_cashTime; //ms
    QDateTime t_time;
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
    QString query2fulljson(const QString & queryStr);
    QString query2jsonstrlist(const QString & queryStr);
    QString query2json(const QString & queryStr);
    bool ConnectDB(const QString &DB_Path,const QString &DB_Name);
    void executeQuery(const QString & queryStr, const QString &query_method,
                      std::function<(QString)> functor=[] (QString jsontext) {});
    void executeQuery(const QString & queryStr,
                      std::function<void(QSqlQuery *query)> functor=[] (QSqlQuery *query) {});
    bool executeProc(const QString & queryStr);
    bool getDbState();
    void receiveText(const QString &query, const QString &query_method);
    void snedReport(const QString &report, const QStringList &emails);

private:
    queryStruc appendQuery(const QString &queryStr, const QString &methodStr="json", int cashTime = DB_CASH_INTERVAL);
    bool queryIsCashed(queryStruc &queryItem);
    bool queryExecute(queryStruc &queryItem);

    queryStruc emptyQuery = {0, QString(), QString(), QString(), QString(), 0, QDateTime()};
    QString errorStr;
    QJSEngine *engine;
    QMap<QString,queryStruc> queryMap;
    QString getQueryLastError(QSqlQuery *query);
    QString str2Json(const QString & name, const QString &val);
    QSqlDatabase DB;
    const QString queryKeyMask = "%1=>%2";


};

#endif // DBWRAPPER_H
