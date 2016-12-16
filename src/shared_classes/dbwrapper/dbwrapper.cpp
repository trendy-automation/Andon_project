#include "dbwrapper.h"
#include <QTime>
#include <QFile>

DBWrapper::DBWrapper(QObject *parent) : QObject(parent)
{
}

DBWrapper::~DBWrapper()
{
}

bool DBWrapper::getDbState()
{
    return DB.open();
}

bool DBWrapper::ConnectDB(const QString &DB_Path,const QString &DB_Name)
{
    if (!QSqlDatabase::contains(DB_Name)){
//        qDebug() << "QSqlDatabase::drivers" << QSqlDatabase::drivers();

//        IBPP::Database ibpp = IBPP::DatabaseFactory("",DB_Name,"andon","andon");
//        ibpp->Connect();

        DB = QSqlDatabase::addDatabase(QLatin1String("QIBASE"),DB_Name); //QIBASE //QFIREBIRD
        DB.setDatabaseName(DB_Path+"/"+DB_Name);
        DB.setUserName("andon");
        DB.setPassword("andon");
        if (!DB.open()) {
            qDebug() << "DB connection failed in ConnectDB";
            qDebug() << DB.lastError().text();
            //qDebug() << "DB Full Path" << DB.databaseName();
            return false;
        }  else         {
            emit DBConnected();
            qDebug() << "DB connected: " << DB.databaseName();
//            DB.close();
//            qDebug() << "DB closed";
        }
    }
    return true;
}

QSqlQuery * DBWrapper::queryexecute (const QString &querytext, QString &lastError)
{
    if (querytext.isEmpty()){
        lastError = "querytext isEmpty";
        qDebug() << lastError;
        return 0;
    }
    if (!DB.open()) {
        lastError = DB.lastError().text();
        qDebug() << querytext << lastError;
        return 0;
    }
    bool trans = QSqlDatabase::database().transaction();
    QSqlQuery * SqlQueryPtr = new QSqlQuery(DB);
//    try{
    if (!SqlQueryPtr->exec(querytext)) {
        lastError=SqlQueryPtr->lastError().text();
        qDebug() << querytext << lastError;
        if (trans)
            QSqlDatabase::database().rollback();
        return 0;
    }
//    QSqlError cur_err1 = QSqlDatabase::database().driver()->lastError();
//        if(cur_err1.isValid())
//            qDebug()<<"driver error"<<cur_err1;
//    QSqlError cur_err2 = QSqlDatabase::database().lastError();
//        if(cur_err2.isValid())
//            qDebug()<<"database error"<<cur_err2;
    if(SqlQueryPtr->lastError().isValid())
        qDebug()<<"Query error"<<SqlQueryPtr->lastError();
//    }
//    catch(IBPP::Exception& e)
//    {
//        qDebug() << "IBPP::Exception" << e.ErrorMessage();
//    }
     QSqlDatabase::database().commit();
    return SqlQueryPtr;
}

void DBWrapper::executeProc(const QString & sqlquery)
{
    QString lastError;
    QSqlQuery *query=queryexecute(sqlquery, lastError);
    if (!query)
        qDebug()<< sqlquery<< "\" faled";

}

QString DBWrapper::query2jsonstrlist(const QString & sqlquery)
{
    QString lastError;
    QSqlQuery *query=queryexecute(sqlquery, lastError);
    if (!query)
        return str2Json("Error", lastError);
    QJsonObject tableObject;
    QStringList FieldList;
    int key_size=QString().number(query->record().count()).size();
    for(int x=0; x < query->record().count(); x++) {
        FieldList << query->record().fieldName(x);
    }
    tableObject["FieldList"]=QJsonValue::fromVariant(FieldList.join(","));
    while(query->next()) {
        QStringList ValueList;
        //TODO: QVariantList ValueList;
        for(int x=0; x < query->record().count(); x++) {
            ValueList << query->value(x).toString().toUtf8();
        }
        tableObject[QString("%1").arg(tableObject.count(), key_size, 10, QChar('0'))]
                =QJsonValue::fromVariant(ValueList.join("|"));
    }
    delete query;
    DB.close();
    QJsonDocument json(tableObject);
    return json.toJson();
}

/*

QString DBWrapper::query2jsonarrays(const QString & sqlquery)
{
    QString lastError;
    QSqlQuery *query=queryexecute(sqlquery, lastError);
    if (!query)
        return str2Json("Error", lastError);

    QJsonObject tableObject;
    QVariantList rowList;
    QVariantList FieldList;
    int key_size=QString().number(query->record().count()).size();
    for(int x=0; x < query->record().count(); x++) {
        FieldList << query->record().fieldName(x);
    }
    //tableObject["FieldList"]=QJsonValue::fromVariant(FieldList.join(","));
    tableObject["FieldList"]=QJsonArray::fromVariantList(FieldList);
    while(query->next()) {
        rowList.clear();
        for(int x=0; x < query->record().count(); x++) {
            rowList.append(query->value(x));
        }
        tableObject[QString("%1").arg(tableObject.count(), key_size, 10, QChar('0'))]
                //          =QJsonValue::fromVariant(rowList.join("|"));
                =QJsonArray::fromVariantList(rowList);
    }
    delete query;
    DB.close();
    QJsonDocument json(tableObject);
    return json.toJson();
}

*/

QString DBWrapper::query2fulljson(const QString &sqlquery)
{
    QTextCodec *codec2 = QTextCodec::codecForName("iso8859-1");
    QString lastError;
    QSqlQuery *query=queryexecute(sqlquery, lastError);
    if (!query)
        return str2Json("Error", lastError);


    QJsonArray rowarray;
    QJsonArray FieldList;
    while(query->next()) {
        QJsonObject jotmp;
        for(int x=0; x < query->record().count(); x++){
            jotmp.insert(codec2->fromUnicode(query->record().fieldName(x)),QJsonValue::fromVariant(query->value(x)));
        }
        QJsonDocument jdtmp(jotmp);
        rowarray.append(QJsonValue::fromVariant(jdtmp.toVariant()));
    }

    for(int x=0; x < query->record().count(); x++)
        FieldList << QJsonValue::fromVariant(codec2->fromUnicode(query->record().fieldName(x)));
    QJsonDocument FieldDoc(FieldList);
    rowarray.append(QJsonValue::fromVariant(FieldDoc.toVariant()));
    QJsonDocument json(rowarray);
    delete query;
    DB.close();
    return json.toJson(QJsonDocument::Compact);
}




QString DBWrapper::query2json(const QString & sqlquery)
{
    QString lastError;
    QSqlQuery *query=queryexecute(sqlquery, lastError);
    if (!query)
        return str2Json("Error", lastError);
    QJsonArray jatmp;
    while(query->next()) {
        QJsonObject jotmp;
        for(int x=0; x < query->record().count(); x++)
            jotmp.insert((query->record().fieldName(x)),QJsonValue::fromVariant(query->value(x)));
        jatmp.append(QJsonValue::fromVariant(QJsonDocument(jotmp).toVariant()));
    }
    QJsonDocument json(jatmp);
    delete query;
    DB.close();
    return json.toJson(QJsonDocument::Compact);
}

void DBWrapper::executeQuery(const QString &sqlquery, const QString &query_method,
                             std::function<void(QString jsontext)> functor)
{
    if (query_method=="query2json")
        return functor(query2json(sqlquery));
    if (query_method=="query2fulljson")
        return functor(query2fulljson(sqlquery));
}

void DBWrapper::executeQuery(const QString & sqlquery,
                             std::function<void(QSqlQuery *query)> functor)
{
    QString lastError;
    QSqlQuery *query=queryexecute(sqlquery, lastError);
    if (!query)
        qDebug()<<lastError;
    else
        return functor(query);
}

QString DBWrapper::getQueryLastError(QSqlQuery *query)
{
    QJsonArray jatmp;
    QJsonObject jotmp;
    jotmp.insert("ERROR",QJsonValue::fromVariant(QVariant(query->lastError().text())));
    QJsonDocument jdtmp(jotmp);
    jatmp.append(QJsonValue::fromVariant(jdtmp.toVariant()));
//    jatmp.append(QJsonValue::fromVariant(QVariant("QSqlQuery ERROR")));
//    jatmp.append(QJsonValue::fromVariant(QVariant(query->lastError().text())));
    jatmp.append(QJsonValue::fromVariant(QVariant("ERROR")));
    return QJsonDocument(jatmp).toJson();
}

QString DBWrapper::str2Json(const QString & name, const QString & val)
{
    QJsonArray jatmp;
    QJsonObject jotmp;
    jotmp.insert(name,QJsonValue::fromVariant(QVariant(val)));
    QJsonDocument jdtmp(jotmp);
    jatmp.append(QJsonValue::fromVariant(jdtmp.toVariant()));
    jatmp.append(QJsonValue::fromVariant(QVariant(name)));
    return QJsonDocument(jatmp).toJson();
}

