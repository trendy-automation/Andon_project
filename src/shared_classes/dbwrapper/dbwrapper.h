#ifndef DBWRAPPER_H
#define DBWRAPPER_H

#include <QObject>
#include <QtSql>
using namespace std;
#include <functional>

class DBWrapper : public QObject
{
    Q_OBJECT
public:
    explicit DBWrapper(QObject *parent = 0);
    ~DBWrapper();

signals:
    void DBConnected();
    void resultQuery(const QString &sql_query, const QString &jsontext);

public slots:
    QString query2fulljson(const QString & sqlquery);
    QString query2jsonstrlist(const QString & sqlquery);
    QString query2json(const QString & sqlquery);
    bool ConnectDB(const QString &DB_Path,const QString &DB_Name);
    QSqlQuery *queryexecute (const QString &querytext, QString &lastError);
    void executeQuery(const QString & sqlquery, const QString &query_method,
                      std::function<void(QString jsontext)> functor=[] (QString jsontext) {});
    void executeQuery(const QString & sqlquery,
                      std::function<void(QSqlQuery *query)> functor=[] (QSqlQuery *query) {});
    void executeProc(const QString & sqlquery);
    bool getDbState();



    //DB.transaction()
private:
    QString getQueryLastError(QSqlQuery *query);
    QString str2Json(const QString & name, const QString &val);
    QSqlDatabase DB;
};

#endif // DBWRAPPER_H
