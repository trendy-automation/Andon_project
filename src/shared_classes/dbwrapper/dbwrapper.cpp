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
    if(!QSqlDatabase::contains(DB_Name)){
//        qDebug() << "QSqlDatabase::drivers" << QSqlDatabase::drivers();

//        IBPP::Database ibpp = IBPP::DatabaseFactory("",DB_Name,"andon","andon");
//        ibpp->Connect();

        DB = QSqlDatabase::addDatabase(QLatin1String("QIBASE"),DB_Name); //QIBASE //QFIREBIRD
        DB.setDatabaseName(DB_Path+"/"+DB_Name);
        DB.setUserName("andon");
        DB.setPassword("andon");
        if(!DB.open()) {
            qDebug() << "DB connection failed in ConnectDB";
            qDebug() << DB.lastError().text();
            return false;
        }  else         {
            emit DBConnected();
            qDebug() << "DB connected: " << DB.databaseName();
            QTimer cleanTimer(this);
            QObject::connect(&cleanTimer, &QTimer::timeout,[this,&cleanTimer](){
                for(auto &q:queries)
                    if(q.time.msecsTo(QDateTime::currentDateTime())>cleanTimer.interval()){
                        q.query->finish();
                        queries.remove(queries.key(q));
                    }
            });
            cleanTimer.start(DB_CASH_CLAEN_INTERVAL);
        }
    }
    return true;
}

QSqlQuery * DBWrapper::queryexecute (const QString &queryText, QString &lastError)
{
    //TODO: queryItem
    if(queryText.isEmpty()){
        lastError = "querytext isEmpty";
        qDebug() << lastError;
        return 0;
    }
    if(!DB.open()) {
        lastError = DB.lastError().text();
        qDebug() << queryText << lastError;
        return 0;
    }
    bool trans = DB.transaction();
    QSqlQuery * sqlQuery;
    if(queries.contains(queryText)){
        if(queries.value(queryText).query->isActive()){
            if(queries.value(queryText).time.msecsTo(QDateTime::currentDateTime())<DB_CASH_INTERVAL)
                return queries.value(queryText).query;
            else
                sqlQuery = queries.value(queryText).query;
        }else{
            queries.insert(queryText,{queries.value(queryText).query,QString(),QString(),QDateTime::currentDateTime()});
            lastError = "query already runing";
            qDebug() << queryText << lastError;
            return 0;
        }
    }else{
        if(queries.count()<DB_QUERIES_LIMIT){
            sqlQuery = new QSqlQuery(queryText,DB);
            queries.insert(queryText,{sqlQuery,QString(),QString(),QDateTime::currentDateTime()});
        }else{
            lastError = "query limit exhausted" << DB_QUERIES_LIMIT;
            qDebug() << queryText << lastError;
            return 0;
        }
    }
//    try{
    if(!sqlQuery->exec()) {
        lastError=sqlQuery->lastError().text();
        qDebug() << queryText << lastError;
        if(trans && sqlQuery->isSelect()){
            sqlQuery->clear();
            DB.rollback();
        }
        return 0;
    }
//    QSqlError cur_err1 = DB.driver()->lastError();
//        if(cur_err1.isValid())
//            qDebug()<<"driver error"<<cur_err1;
//    QSqlError cur_err2 = DB.lastError();
//        if(cur_err2.isValid())
//            qDebug()<<"database error"<<cur_err2;
    if(sqlQuery->lastError().isValid())
        qDebug()<<"sqlQuery->lastError().isValid()"<<sqlQuery->lastError();
//    }
//    catch(IBPP::Exception& e)
//    {
//        qDebug() << "IBPP::Exception" << e.ErrorMessage();
//    }
     //DB.commit();
    if(sqlQuery->isSelect()){
        sqlQuery->finish();
        DB.commit();
    }

    return sqlQuery;
}

void DBWrapper::executeProc(const QString & sqlquery)
{
    QString lastError;
    QSqlQuery *query=queryexecute(sqlquery, lastError);
    if(!query)
        qDebug()<< sqlquery<< "\" faled";

}

QString DBWrapper::query2jsonstrlist(const QString & sqlquery)
{
    QString lastError;
    QSqlQuery *query=queryexecute(sqlquery, lastError);
    if(!query)
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
    if(!query)
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
    if(!query)
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
    if(!query)
        return str2Json("Error", lastError);
    QJsonArray jatmp;
    while(query->next()) {
        QJsonObject jotmp;
        for(int x=0; x < query->record().count(); x++){
            //TODO DateTime
//            qDebug()<<query->value(x).userType();
//            if(query->value(x).userType()==QVariant::DateTime){
//                QString timestamp = query->value(x).toDateTime().toString("dd.MM.yy hh:mm:ss.zzz");
//                qDebug()<<"timestamp"<<timestamp;
//                jotmp.insert((query->record().fieldName(x)),QJsonValue::fromVariant(timestamp));
//            }
//            else
                jotmp.insert((query->record().fieldName(x)),QJsonValue::fromVariant(query->value(x)));
        }
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
    if(query_method=="query2json")
        return functor(query2json(sqlquery));
    if(query_method=="query2fulljson")
        return functor(query2fulljson(sqlquery));
}

void DBWrapper::executeQuery(const QString & sqlquery,
                             std::function<void(QSqlQuery *query)> functor)
{
    QString lastError;
    QSqlQuery *query=queryexecute(sqlquery, lastError);
    if(!query)
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



void DBWrapper::receiveText(const QString &query, const QString &query_method)
{
////        qDebug() << "sql_query"<<sql_query;
////                 << "query_method" << query_method << "queries.count()" << queries.count()
////                 << "recive_count" << recive_count;
//    QString sql_query=QString(query).toUpper();
//    if(queries.contains(sql_query)) {
//        if(queries.value(sql_query).pageTime.elapsed()<DB_CASH_INTERVAL) {
////                qDebug() << "queries.contains(sql_query)";
////                         << queries.count()<<recive_count;
//            if(!queries.value(sql_query).pageData.isEmpty()){
////                    qDebug() << "emit sendText";
//                emit sendText(sql_query, queries.value(sql_query).pageData);
//                return;
//            }
//            else {
//                qDebug() << "pageData.isEmpty()";
//            }
//        }
//    } else {
//        queries.insert(sql_query,{QString(),QTime::currentTime()});
//    }
//    getSqlQuery(sql_query, query_method, [this, sql_query](QString jsontext){
//        queries.insert(sql_query,{jsontext,QTime::currentTime()});
//        emit sendText(sql_query, jsontext);
//    });

}
void DBWrapper::snedReport(const QString &report, const QStringList &emails)
{
//    QString sql_query;
//    QString template_file;
//    QString res_file="report.xlsx";
//    QString subject;
//    QString sheet;
//    //SELECT p.DESCRIPTION
//    //FROM DB_REPORT_DESCRIPTION(PROCEDURE_NAME) p
//    if(report==QString("Daily_PDP")) {
//        sql_query=QString("SELECT * FROM PRODUCTION_REPORT_SHIFT_PDP");
//        res_file=QString("PDP %1.xlsx").arg(QDate::currentDate().toString("ddd d MMMM"));
//        subject=QString("Производственный отчёт %1 за %2 смену ")
//                .arg(QDate::currentDate().toString("ddd d MMMM")).arg(QTime::currentTime().hour()<15?1:2);
//    }
//    if(report==QString("Weekly_PDP")) {
//        sql_query="SELECT * FROM PRODUCTION_REPORT_PDP";
//        template_file="PDP.xlsx";
//        res_file=QString("PDP W%1.xlsx").arg(QDate::currentDate().weekNumber());
//        subject=QString("Производственный отчёт большие тримы за неделю %1").arg(QDate::currentDate().weekNumber()) ;
//    }
//    if(report==QString("REPORT_BREAKDOWNS")) {
//        sql_query="SELECT * FROM REPORT_BREAKDOWNS";
//        res_file=QString("REPORT_BREAKDOWNS_%1.xlsx").arg(QDate::currentDate().toString("dd_MM_yyyy"));
//        subject=QString("Простои производства %1").arg(QDate::currentDate().toString("ddd d MMMM"));
//        sheet=QString("Отчёт по простоям %1").arg(QDate::currentDate().toString("ddd d MMMM"));
//    }

//    qDebug()<<"getSqlQuery"<<report<<emails<<sql_query;
//    getSqlQuery(sql_query, [this,&subject,&sheet,&template_file,&res_file,emails]
//                    (QSqlQuery *query){
//            QXlsx::Document * xlsx;
//            if(template_file.isEmpty())
//                xlsx = new QXlsx::Document();
//            else
//                xlsx = new QXlsx::Document(template_file);
//            xlsx->addSheet(sheet);
//            int i=1;
//            QTextCodec *codec = QTextCodec::codecForName("iso8859-1");
//            for(int j=0; j < query->record().count(); j++)
//                xlsx->write(i,j+1,QString(codec->fromUnicode(query->record().fieldName(j))));
//            while(query->next()) {
//                i++;
//                for(int j=0; j < query->record().count(); j++)
//                    xlsx->write(i,j+1,query->value(j));
//            }
//            if(i>1){
//                QBuffer buffer(new QByteArray);
//                xlsx->saveAs(&buffer);
//                buffer.setProperty("FILE_NAME",res_file);
//                emit sendReport2email(subject,"",emails,&(QList<QBuffer*>()<<&buffer));
//            }
//        });
}
