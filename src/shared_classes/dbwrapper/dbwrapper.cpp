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
        }else{
            qDebug() << "DB connected: " << DB.databaseName();
            QTimer *cleanTimer = new QTimer(this);
            QObject::connect(cleanTimer, &QTimer::timeout,[this,cleanTimer](){
                for(auto &q:queryMap){
                    qDebug() << 1;
                    qDebug() << q.t_time;
                    qDebug() << q.t_time.msecsTo(QDateTime::currentDateTime());
                    if(q.t_time.msecsTo(QDateTime::currentDateTime())>cleanTimer->interval()){
                        qDebug() << 2;
                        if(q.p_query){
                            qDebug() << 3;
                            if(q.p_query->isActive()){
                                qDebug() << 4;
                                q.p_query->finish();
                            }
                        }
                        queryMap.remove(QString(queryKeyMask).arg(q.s_sql_query).arg(q.s_method));
                    }
                }
                qDebug() << "queryMap.count()" << queryMap.count();
                if(queryMap.isEmpty()){
                    qDebug() << 5;
                    DB.close();
                }
            });
            cleanTimer->start(DB_CASH_CLAEN_INTERVAL);
            emit DBConnected();
        }
    }
    return true;
}

queryStruc DBWrapper::appendQuery(const QString &queryStr,const QString &methodStr, int cashTime)
{
    QString key=QString(queryKeyMask).arg(queryStr).arg(methodStr);
    if(!queryMap.contains(queryStr)){
        if(queryMap.count()>DB_QUERIES_LIMIT)
            return
            //queryMap.insert(key,
            {0, queryStr, methodStr, QString(), QString("query`s limit(%1) exhausted").arg(DB_QUERIES_LIMIT),
             cashTime, QDateTime::currentDateTime()};//);
        else
            queryMap.insert(key,
            {0, queryStr, methodStr, QString(), QString(),
             cashTime, QDateTime::currentDateTime()});
    }
    qDebug()<<"queryMap.count()"<<queryMap.count();
    return queryMap.value(key);
    /*    if(queryMap.contains(queryStr)){
        if(queryMap.value(queryText).p_query->isActive()){
            if(queryMap.value(queryStr).t_time.msecsTo(QDateTime::currentDateTime())<cashTime)
                return queryMap.value(queryStr);
            else
                queryItem = queryMap.value(queryStr);
        }else{
            queryMap.insert(queryStr,{queryMap.value(queryStr).p_query,QString(),QString(),QDateTime::currentDateTime()});
            errorStr = "query already runing";
        }
    }else{
        if(queryMap.count()<DB_QUERIES_LIMIT){
            queryMap.insert(queryStr,{new QSqlQuery(queryStr,DB), QString(), QString(), QString(), cashTime,
                                       QDateTime::currentDateTime()});
        }else{
            errorStr = QString("query`s limit(%1) exhausted").arg(DB_QUERIES_LIMIT);
        }
    }
    return queryMap.value(queryStr);
*/
}

bool DBWrapper::queryIsCashed(queryStruc &queryItem)
{
    if(!queryItem.s_error.isEmpty())
        if(queryItem.p_query)
            if(!queryItem.p_query->executedQuery().isEmpty())
                if(queryItem.t_time.msecsTo(QDateTime::currentDateTime())<queryItem.i_cashTime)
                    return true;
    return false;
}

bool DBWrapper::queryExecute (queryStruc &queryItem)
{
//    qDebug() << queryItem.i_cashTime << queryItem.j_result << queryItem.p_query << queryItem.s_error
//                << queryItem.s_method << queryItem.s_sql_query << queryItem.t_time;
    if(queryItem.s_sql_query.isEmpty())
        queryItem.s_error = "Query is empty";
    if(!DB.open())
        queryItem.s_error = DB.lastError().text();

//    if(queryItem.p_query->lastError().isValid())
//        qDebug()<<"queryItem.p_query->lastError().isValid()"<<queryItem.p_query->lastError();

/*
        if(queryMap.contains(queryText)){
            if(queryMap.value(queryText).p_query->isActive()){
                if(queryMap.value(queryText).t_time.msecsTo(QDateTime::currentDateTime())<DB_CASH_INTERVAL)
                    return queryMap.value(queryText);
                else
                    queryItem = queryMap.value(queryText);
            }else{
                queryMap.insert(queryText,{queryMap.value(queryText).p_query,QString(),QString(),QDateTime::currentDateTime()});
                queryItem.s_error = "query already runing";
            }
        }else{
            if(queryMap.count()<DB_QUERIES_LIMIT){
                queryItem.p_query = new QSqlQuery(queryText,DB);
                queryMap.insert(queryText,{queryItem.p_query,QString(),QString(),QDateTime::currentDateTime()});
            }else{
                queryItem.s_error = QString("query`s limit(%1) exhausted").arg(DB_QUERIES_LIMIT);
            }
        }
*/
    if(queryItem.s_error.isEmpty()){
        //    try{
        DB.transaction();
        if(!queryItem.p_query)
            queryItem.p_query = new QSqlQuery(DB);
        if(queryItem.p_query->exec(queryItem.s_sql_query)){
            //DB.commit();
            if(queryItem.p_query->lastError().isValid())
                qDebug()<<"queryItem.p_query->lastError().isValid()"<<queryItem.p_query->lastError();
            return true;
        }
        //    }
        //    catch(IBPP::Exception& e)
        //    {
        //        qDebug() << "IBPP::Exception" << e.ErrorMessage();
        //    }
        queryItem.s_error=queryItem.p_query->lastError().text();
        //qDebug() << QString("Error in query:\"%1\" - %2").arg(queryItem.s_sql_query).arg(queryItem.s_error);
        DB.rollback();
    }
    return false;
}

bool DBWrapper::executeProc(const QString & queryStr)
{
    queryStruc queryItem = appendQuery(queryStr,"",0);
    if(queryExecute(queryItem)){
        queryItem.p_query->finish();
        DB.commit();
        return true;
    }
    else
        qDebug() << QString("Error in query:\"%1\" - %2").arg(queryItem.s_sql_query).arg(queryItem.s_error);
    return false;
}

QString DBWrapper::query2jsonstrlist(const QString & queryStr)
{
    queryStruc queryItem = appendQuery(queryStr,"jsonstrlist");
    if(queryIsCashed(queryItem))
        return queryItem.j_result;
    if(queryExecute(queryItem)){
        QJsonObject tableObject;
        QStringList FieldList;
        int key_size=QString().number(queryItem.p_query->record().count()).size();
        for(int x=0; x < queryItem.p_query->record().count(); x++) {
            FieldList << queryItem.p_query->record().fieldName(x);
        }
        tableObject["FieldList"]=QJsonValue::fromVariant(FieldList.join(","));
        while(queryItem.p_query->next()) {
            QStringList ValueList;
            //TODO: QVariantList ValueList; // for date, null, ets
            for(int x=0; x < queryItem.p_query->record().count(); x++) {
                ValueList << queryItem.p_query->value(x).toString().toUtf8();
            }
            tableObject[QString("%1").arg(tableObject.count(), key_size, 10, QChar('0'))]
                    =QJsonValue::fromVariant(ValueList.join("|"));
        }
        queryItem.p_query->finish();
        DB.commit();
        QJsonDocument json(tableObject);
        return json.toJson();
    }
    qDebug() << QString("Error in query:\"%1\" - %2").arg(queryItem.s_sql_query).arg(queryItem.s_error);
    return str2Json("Error", queryItem.s_error);
}

/*

QString DBWrapper::query2jsonarrays(const QString & queryStr)
{
    QString lastError;
    QSqlQuery *query=queryexecute(queryStr, lastError);
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

QString DBWrapper::query2fulljson(const QString &queryStr)
{
    QTextCodec *codec2 = QTextCodec::codecForName("iso8859-1");
    queryStruc queryItem = appendQuery(queryStr,"fulljson");
    if(queryIsCashed(queryItem))
        return queryItem.j_result;
    if(queryExecute(queryItem)){
        QJsonArray rowarray;
        QJsonArray FieldList;
        while(queryItem.p_query->next()) {
            QJsonObject jotmp;
            for(int x=0; x < queryItem.p_query->record().count(); x++){
                jotmp.insert(codec2->fromUnicode(queryItem.p_query->record().fieldName(x)),QJsonValue::fromVariant(queryItem.p_query->value(x)));
            }
            QJsonDocument jdtmp(jotmp);
            rowarray.append(QJsonValue::fromVariant(jdtmp.toVariant()));
        }
        for(int x=0; x < queryItem.p_query->record().count(); x++)
            FieldList << QJsonValue::fromVariant(codec2->fromUnicode(queryItem.p_query->record().fieldName(x)));
        queryItem.p_query->finish();
        DB.commit();
        QJsonDocument FieldDoc(FieldList);
        rowarray.append(QJsonValue::fromVariant(FieldDoc.toVariant()));
        QJsonDocument json(rowarray);
        return json.toJson(QJsonDocument::Compact);
    }
    qDebug() << QString("Error in query:\"%1\" - %2").arg(queryItem.s_sql_query).arg(queryItem.s_error);
    return str2Json("Error", queryItem.s_error);
}




QString DBWrapper::query2json(const QString & queryStr)
{
    queryStruc queryItem = appendQuery(queryStr,"json");
    if(queryIsCashed(queryItem))
        return queryItem.j_result;
    if(queryExecute(queryItem)){
        QJsonArray jatmp;
        while(queryItem.p_query->next()) {
            QJsonObject jotmp;
            for(int x=0; x < queryItem.p_query->record().count(); x++){
                //TODO DateTime
                //            qDebug()<<queryItem.p_query->value(x).userType();
                //            if(queryItem.p_query->value(x).userType()==QVariant::DateTime){
                //                QString timestamp = queryItem.p_query->value(x).toDateTime().toString("dd.MM.yy hh:mm:ss.zzz");
                //                qDebug()<<"timestamp"<<timestamp;
                //                jotmp.insert((queryItem.p_query->record().fieldName(x)),QJsonValue::fromVariant(timestamp));
                //            }
                //            else
                jotmp.insert((queryItem.p_query->record().fieldName(x)),QJsonValue::fromVariant(queryItem.p_query->value(x)));
            }
            jatmp.append(QJsonValue::fromVariant(QJsonDocument(jotmp).toVariant()));
        }
        QJsonDocument json(jatmp);
        DB.commit();
        return json.toJson(QJsonDocument::Compact);
    }
    //qDebug() << QString("Error in query:/'%1/' - %2").arg(queryStr).arg(queryItem.s_error);
    return QString(); //str2Json("Error", queryItem.s_error);
}

void DBWrapper::executeQuery(const QString &queryStr, const QString &query_method,
                             std::function<void(QString jsontext)> functor)
{
    if(query_method=="query2json")
        return functor(query2json(queryStr));
    if(query_method=="query2fulljson")
        return functor(query2fulljson(queryStr));
}

void DBWrapper::executeQuery(const QString & queryStr,
                             std::function<void(QSqlQuery *query)> functor)
{
    queryStruc queryItem = appendQuery(queryStr,"",0);
    if(queryExecute(queryItem))
        return functor(queryItem.p_query);
    qDebug() << QString("Error in query:\"%1\" - %2").arg(queryItem.s_sql_query).arg(queryItem.s_error);
    //return str2Json("Error", queryItem.s_error);
    return functor(0);
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
