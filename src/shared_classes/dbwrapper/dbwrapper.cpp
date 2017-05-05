#include "dbwrapper.h"

DBWrapper::DBWrapper(QObject *parent) : QObject(parent)
{
    packFunctionsMap.insert("",[this](QSqlQuery*sqlQuery){return QString();});
    packFunctionsMap.insert("json",[this](QSqlQuery*sqlQuery){
        //qDebug() << "packFunctionsMap json sqlQuery" << sqlQuery;
        QJsonArray jatmp;
        while(sqlQuery->next()) {
            QJsonObject jotmp;
            for(int x=0; x < sqlQuery->record().count(); x++)
                jotmp.insert((sqlQuery->record().fieldName(x)),QJsonValue::fromVariant(sqlQuery->value(x)));
            jatmp.append(QJsonValue::fromVariant(QJsonDocument(jotmp).toVariant()));
        }
        sqlQuery->finish();
        DB.commit();
        QJsonDocument json(jatmp);
//        qDebug() << "packFunctionsMap json toJson" << json.toJson(QJsonDocument::Compact);
        return json.toJson(QJsonDocument::Compact);
    });
    packFunctionsMap.insert("fulljson",[this](QSqlQuery*sqlQuery){
        QJsonArray jatmp;
        QTextCodec *codec2 = QTextCodec::codecForName("iso8859-1");
        QJsonArray FieldList;
        while(sqlQuery->next()) {
            QJsonObject jotmp;
            for(int x=0; x < sqlQuery->record().count(); x++)
                jotmp.insert(codec2->fromUnicode(sqlQuery->record().fieldName(x)),QJsonValue::fromVariant(sqlQuery->value(x)));
            QJsonDocument jdtmp(jotmp);
            jatmp.append(QJsonValue::fromVariant(jdtmp.toVariant()));
        }
        for(int x=0; x < sqlQuery->record().count(); x++)
            FieldList << QJsonValue::fromVariant(codec2->fromUnicode(sqlQuery->record().fieldName(x)));
        sqlQuery->finish();
        DB.commit();
        QJsonDocument FieldDoc(FieldList);
        jatmp.append(QJsonValue::fromVariant(FieldDoc.toVariant()));
        QJsonDocument json(jatmp);
        return json.toJson(QJsonDocument::Compact);
    });
    packFunctionsMap.insert("jsonstrlist",[this](QSqlQuery*sqlQuery){
        QJsonArray jatmp;
        QJsonObject tableObject;
        QStringList FieldList;
        int key_size=QString().number(sqlQuery->record().count()).size();
        for(int x=0; x < sqlQuery->record().count(); x++) {
            FieldList << sqlQuery->record().fieldName(x);
        }
        tableObject["FieldList"]=QJsonValue::fromVariant(FieldList.join(","));
        while(sqlQuery->next()) {
            QStringList ValueList;
            //TODO: QVariantList ValueList; // for date, null, ets
            for(int x=0; x < sqlQuery->record().count(); x++)
                ValueList << sqlQuery->value(x).toString().toUtf8();
            tableObject[QString("%1").arg(tableObject.count(), key_size, 10, QChar('0'))]
                    =QJsonValue::fromVariant(ValueList.join("|"));
        }
        //TODO tableObject to QJsonDocument
        sqlQuery->finish();
        DB.commit();
        QJsonDocument json(jatmp);
        return json.toJson(QJsonDocument::Compact);
    });
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
    //        qDebug() << "QSqlDatabase::drivers" << QSqlDatabase::drivers();
    //        IBPP::Database ibpp = IBPP::DatabaseFactory("",DB_Name,"andon","andon");
    //        ibpp->Connect();
    if(!QSqlDatabase::contains(DB_Name)){
        DB = QSqlDatabase::addDatabase(QLatin1String("QIBASE"),DB_Name); //QFIREBIRD
        DB.setDatabaseName(DB_Path+QDir::separator()+DB_Name);
        DB.setUserName("andon");
        DB.setPassword("andon");
    }
    if(DB.open()){
        qDebug() << "DB connected: " << DB.databaseName();
        dbOK=1; //TODO delete?
        QTimer *cleanTimer = new QTimer(this);
        QObject::connect(cleanTimer, &QTimer::timeout,[this,cleanTimer](){
            QMapIterator<QString,queryStruct> q(queryMap);
            while (q.hasNext()) {
                q.next();
                if(!q.value().i_cashTime==0){
                    int elapsed = q.value().t_time.msecsTo(QDateTime::currentDateTime());
                    if(elapsed<cleanTimer->interval() || elapsed<q.value().i_cashTime)
                        continue;
                }
                    queryMap.remove(q.key());
            }
            //qDebug() << "queryMap.count()" << queryMap.count();
            if(queryMap.isEmpty()){
                DB.close();
            }
        });
        cleanTimer->start(DB_CASH_CLAEN_INTERVAL);
        emit DBConnected();
        return true;
    }
    qDebug() << "DB connection failed" << DB.lastError().text();
    return false;
}

queryStruct DBWrapper::appendQuery(const QString &queryText, const QString &queryMethod, int cashTime)
{
    QString key=QString(queryKeyMask).arg(queryText).arg(queryMethod);
    QString error;
    if(queryMap.count()>DB_QUERIES_LIMIT)
        error=QString("query`s limit(%1) exhausted").arg(DB_QUERIES_LIMIT);
    if(!packFunctionsMap.contains(queryMethod))
        error=QString("packFunctionsMap not contains method:%1!").arg(queryMethod);
    if(!error.isEmpty())
        return {0, queryText, queryMethod, QString(), error, cashTime, QDateTime::currentDateTime()};
    queryMap.insert(key,{0, queryText, queryMethod, QString(), QString(), cashTime, QDateTime::currentDateTime()});
    return queryMap.value(key);
}

bool DBWrapper::queryIsCashed(queryStruct &queryItem)
{
    if(!queryItem.s_error.isEmpty())
        if(queryItem.p_query)
            if(!queryItem.p_query->executedQuery().isEmpty())
                if(queryItem.i_cashTime!=0)
                    if(queryItem.t_time.msecsTo(QDateTime::currentDateTime())<queryItem.i_cashTime)
                        return true;
    return false;
}

bool DBWrapper::queryExecute (queryStruct &queryItem)
{
    //    qDebug() << queryItem.i_cashTime << queryItem.j_result << queryItem.p_query << queryItem.s_error
    //                << queryItem.s_method << queryItem.s_sql_query << queryItem.t_time;
    if(queryItem.s_sql_query.isEmpty()){
        queryItem.s_error = "Query is empty";
        return false;
    }
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
        queryItem.p_query = new QSqlQuery(DB);
        if(queryItem.p_query->exec(queryItem.s_sql_query))
            if(!queryItem.p_query->lastError().isValid())
                return true;
            /*                {
                qDebug()<<"queryItem.p_query->lastError().isValid()"<<queryItem.p_query->lastError();
            else{
                errorCounter=0;

            }
        }
            }
            catch(IBPP::Exception& e)
            {
                qDebug() << "IBPP::Exception" << e.ErrorMessage();
            }*/
            queryItem.s_error=queryItem.p_query->lastError().text();
            DB.rollback();
            if(queryItem.s_error.contains("Could not prepare statement")>0)
                queryItem.i_cashTime=+60000;
    }
    errorCounter=+dbOK;
    if(errorCounter>signalErrorCount){
        qDebug() << "errorCounter>signalErrorCount emit dbError" << queryItem.s_error;
        emit dbError(queryItem.s_error);
    }
    return false;
}

bool DBWrapper::executeProc(const QString & queryText)
{
    queryStruct queryItem = appendQuery(queryText,"",0);
    if(queryExecute(queryItem)){
        queryItem.p_query->finish();
        DB.commit();
        return true;
    }
    qDebug() << QString("Error in query:\"%1\" - %2").arg(queryItem.s_sql_query).arg(queryItem.s_error);
    return false;
}

/*QString DBWrapper::query2jsonstrlist(const QString & queryText, int cashTime)
{
    queryStruct queryItem = appendQuery(queryText,JSONStrList,cashTime);
    if(queryIsCashed(queryItem))
        return queryItem.s_result;
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
}*/

/*QString DBWrapper::query2jsonarrays(const QString & queryText)
{
    QString lastError;
    QSqlQuery *query=queryexecute(queryText, lastError);
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

    QJsonDocument json(tableObject);
    return json.toJson();
}

*/

/*QString DBWrapper::query2fulljson(const QString &queryText, int cashTime)
{
    queryStruct queryItem = appendQuery(queryText,fullJSON,cashTime);
    if(queryIsCashed(queryItem))
        return queryItem.s_result;
    if(queryExecute(queryItem)){
        QJsonArray jatmp;
        QTextCodec *codec2 = QTextCodec::codecForName("iso8859-1");
        QJsonArray FieldList;
        while(queryItem.p_query->next()) {
            QJsonObject jotmp;
            for(int x=0; x < queryItem.p_query->record().count(); x++){
                jotmp.insert(codec2->fromUnicode(queryItem.p_query->record().fieldName(x)),QJsonValue::fromVariant(queryItem.p_query->value(x)));
            }
            QJsonDocument jdtmp(jotmp);
            jatmp.append(QJsonValue::fromVariant(jdtmp.toVariant()));
        }
        for(int x=0; x < queryItem.p_query->record().count(); x++)
            FieldList << QJsonValue::fromVariant(codec2->fromUnicode(queryItem.p_query->record().fieldName(x)));
        queryItem.p_query->finish();
        DB.commit();

        QJsonDocument FieldDoc(FieldList);
        jatmp.append(QJsonValue::fromVariant(FieldDoc.toVariant()));
        QJsonDocument json(jatmp);
        return json.toJson(QJsonDocument::Compact);
    }
    qDebug() << QString("Error in query:\"%1\" - %2").arg(queryItem.s_sql_query).arg(queryItem.s_error);
    return str2Json("Error", queryItem.s_error);
}*/

/*QString DBWrapper::query2json(const QString & queryText, int cashTime)
{
    queryStruct queryItem = appendQuery(queryText,JSON,cashTime);
    if(queryIsCashed(queryItem))
        return queryItem.s_result;
    if(queryExecute(queryItem)){
        QJsonArray jatmp;
        while(queryItem.p_query->next()) {
            QJsonObject jotmp;
            for(int x=0; x < queryItem.p_query->record().count(); x++){
//                TODO DateTime
//                            qDebug()<<queryItem.p_query->value(x).userType();
//                            if(queryItem.p_query->value(x).userType()==QVariant::DateTime){
//                                QString timestamp = queryItem.p_query->value(x).toDateTime().toString("dd.MM.yy hh:mm:ss.zzz");
//                                qDebug()<<"timestamp"<<timestamp;
//                                jotmp.insert((queryItem.p_query->record().fieldName(x)),QJsonValue::fromVariant(timestamp));
//                            }
//                            else
                jotmp.insert((queryItem.p_query->record().fieldName(x)),QJsonValue::fromVariant(queryItem.p_query->value(x)));
            }
            jatmp.append(QJsonValue::fromVariant(QJsonDocument(jotmp).toVariant()));
        }
        QJsonDocument json(jatmp);
        DB.commit();

        return json.toJson(QJsonDocument::Compact);
    }
    //qDebug() << QString("Error in query:/'%1/' - %2").arg(queryText).arg(queryItem.s_error);
    return QString(); //str2Json("Error", queryItem.s_error);
}
*/

/*QString DBWrapper::query2jsonstrlist(const QString & queryText, int cashTime)
{
    return query2method(queryText,"jsonstrlist",cashTime);
}

QString DBWrapper::query2fulljson(const QString &queryText, int cashTime)
{
    return query2method(queryText,"fulljson",cashTime);
}*/

QString DBWrapper::cashedQuery(const QString & queryText, int cashTime)
{
    return query2method(queryText,"json",cashTime);
}

QString DBWrapper::query2jsonstrlist(const QString & queryText)
{
    return query2method(queryText,"jsonstrlist",0);
}

QString DBWrapper::query2fulljson(const QString &queryText)
{
    return query2method(queryText,"fulljson",0);
}

QString DBWrapper::query2json(const QString & queryText)
{
    return query2method(queryText,"json",0);
}

QString DBWrapper::query2method(const QString & queryText, const QString &queryMethod, int cashTime)
{
    //qDebug() << queryText << queryMethod << cashTime;
    queryStruct queryItem = appendQuery(queryText,queryMethod,cashTime);
    if(queryIsCashed(queryItem)){
        qDebug() << "queryIsCashed";
        return queryItem.s_result;
    }
    if(queryExecute(queryItem)){
        //qDebug() << "queryExecute OK";
        if(!packFunctionsMap.contains(queryItem.s_method))
             queryItem.s_error=QString("Undefined query method(%1)!").arg(queryItem.s_method);
        else{
            queryItem.s_result = packFunctionsMap.value(queryItem.s_method)(queryItem.p_query);
            return queryItem.s_result;
        }
    }
    qDebug() << QString("Error in query:\"%1\"\r\n%2").arg(queryItem.s_sql_query).arg(queryItem.s_error);
    if(queryItem.s_method=="json")
        return QString();
    return str2Json("Error", queryItem.s_error);
}

void DBWrapper::executeQuery(const QString &queryText, const QString &query_method,
                             std::function<void(QString jsontext)> functor)
{
    if(query_method.compare("query2json")==0)
        functor(query2json(queryText));
    if(query_method.compare("query2fulljson")==0)
        functor(query2fulljson(queryText));
}

void DBWrapper::executeQuery(const QString & queryText,
                             std::function<void(QSqlQuery *)> functor)
{
    queryStruct queryItem = appendQuery(queryText,"",0);
    if(queryExecute(queryItem)){
        functor(queryItem.p_query);
        queryItem.p_query->finish();
        DB.commit();
        return;
    }
    qDebug() << QString("Error in query:\"%1\" - %2").arg(queryItem.s_sql_query).arg(queryItem.s_error);
    return;
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
