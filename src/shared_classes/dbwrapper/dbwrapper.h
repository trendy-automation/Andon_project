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

struct queryTextuc{
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
    QString query2fulljson(const QString & queryText);
    QString query2jsonstrlist(const QString & queryText);
    QString query2json(const QString & queryText);
    bool ConnectDB(const QString &DB_Path,const QString &DB_Name);
    void executeQuery(const QString & queryText, const QString &query_method,
                      std::function<void(QString)> functor=[] (QString jsontext) {});
    void executeQuery(const QString & queryText,
                      std::function<void(QSqlQuery*)> functor=[] (QSqlQuery *query) {});
    bool executeProc(const QString & queryText);
    bool getDbState();
    void receiveText(const QString &query, const QString &query_method);
    void snedReport(const QString &report, const QStringList &emails);

private:
    queryTextuc appendQuery(const QString &queryText, const QString &methodStr="json", int cashTime = DB_CASH_INTERVAL);
    bool queryIsCashed(queryTextuc &queryItem);
    bool queryExecute(queryTextuc &queryItem);

    queryTextuc emptyQuery = {0, QString(), QString(), QString(), QString(), 0, QDateTime()};
    QString errorStr;
    QJSEngine *engine;
    QMap<QString,queryTextuc> queryMap;
    QString getQueryLastError(QSqlQuery *query);
    QString str2Json(const QString & name, const QString &val);
    QSqlDatabase DB;
    const QString queryKeyMask = "%1=>%2";


};

#endif // DBWRAPPER_H
