#ifndef CLIENTWEBINTERFACE_H
#define CLIENTWEBINTERFACE_H

//#include <QThread>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QDebug>
#include <QTime>
#include <QTimer>

#include <QJSEngine>
#include <functional>



class ClientWebInterface : public QObject //QThread
{
    Q_OBJECT
public:
    ClientWebInterface(QObject *parent=0)    : QObject(parent) //QThread(parent)
    {
    }
signals:

public slots:


    void setEngine(QJSEngine* SharedEngine)
    {
        engine=SharedEngine;
    }

    QVariant evaluate(const QString &script)
    {
        if(engine){
            QJSValue result = engine->evaluate(script);
            if (result.isError())
                qDebug() << "Uncaught exception at line"
                         << result.property("lineNumber").toInt()
                         << ":" << result.toString();
            qDebug()<<result.toVariant();
            if (result.isVariant())
                return result.toVariant();
        }
        return QVariant();
    }

//    void ClientExecute(QScriptValue scriptFunctor)
//    {
//        std::function<void(QVariant response)> functor;
//        if(scriptFunctor.isFunction())
//            functor = [scriptFunctor,this] (QVariant response) {
//                QScriptValue(scriptFunctor).call(
//                        engine->globalObject(),QScriptValueList() << engine->toScriptValue(response));
//            };
//        else
//            functor=[] (QVariant response) { qDebug()<<"scriptFunctor is not a function."<<response; };
//        return functor;
//    }


private:
    QJSEngine *engine;

};
#endif // CLIENTWEBINTERFACE_H
