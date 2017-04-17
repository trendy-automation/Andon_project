#ifndef WATCHDOG_H
#define WATCHDOG_H
#include <QApplication>
#include <QProcess>
#include <QTcpSocket>
#include <QTimer>
#include "qjsonrpcservicereply.h"
#include "qjsonrpcsocket.h"

class Watchdog: public QObject
{
    Q_OBJECT
public:
    Watchdog(QObject *parent=0)
        : QObject(parent)
    {

    }
    bool listen(int rpcPort, const QString &aliveMethod)
    {
        QTcpSocket *socket = new QTcpSocket;
        qDebug() << "connect socket, &QTcpSocket::connected"<<socket;
        QObject::connect(socket, &QTcpSocket::connected,[socket,aliveMethod](){
            qDebug() << "Run application watchdog"<<socket;
            QJsonRpcSocket *m_client = new QJsonRpcSocket(socket);
            QTimer *watchTimer = new QTimer;
            qDebug() << "connect watchTimer timeout";
            QObject::connect(watchTimer, &QTimer::timeout, [m_client,watchTimer,socket,aliveMethod] () {
                QJsonRpcServiceReply *reply = m_client->invokeRemoteMethod(aliveMethod);
                QTimer *replayTimer=new QTimer;
                replayTimer->setSingleShot(true);
                QObject::connect(replayTimer, &QTimer::timeout, [] () {
                    qDebug() << "Force restart application(response timeout)!";
                    //replayTimer->stop();
                    //replayTimer->deleteLater();
                    //watchTimer->stop();
                    //watchTimer->deleteLater();
                    //m_client->deleteLater();
                    QProcess *restartApp = new QProcess;
                    QObject::connect(restartApp,&QProcess::errorOccurred,[](QProcess::ProcessError error){
                        qDebug() << "restartApp errorOccurred!"<<error;
                    });
                    /*QProcess* watchdogProcess = a.findChild<QProcess*>("watchdogProcess");
                    if(watchdogProcess)
                        watchdogProcess->kill();
                    QJsonRpcTcpServer* rpcServer = a.findChild<QJsonRpcTcpServer*>("rpcServer");
                    if(rpcServer)
                        rpcServer->close();
                    QList<QTimer*> timerList = a.findChildren<QTimer*>();
                    qDebug() << "Stop and delete timers" << timerList.count();
                    for(auto &t:timerList){
                        if(t->isActive())
                            t->stop();
                        t->deleteLater();
                    }
                    a.quit();*/
                    restartApp->start("cmd.exe",QStringList()<<"/C"<<"start"<<qApp->applicationFilePath().at(0)<<"force");
                });
                replayTimer->start(1000);
                QObject::connect(reply, &QJsonRpcServiceReply::finished, [reply,replayTimer,watchTimer] () {
                    replayTimer->stop();
                    replayTimer->deleteLater();
                    //qDebug() << "alive" << reply->response().result().toVariant().toBool();
                    /*
                        if (!reply->response().result().toVariant().isValid()){
                            watchTimer->stop();
                            //qDebug() << "invalid response received!!!" << reply->response().errorMessage();
                            qDebug() << "Force restart application(invalid response)";
                            QProcess *restartApp = new QProcess;
                            QObject::connect(restartApp,&QProcess::started,[restartApp](){
                                qDebug() << "restartApp started";
                                qDebug() << "restartApp->pid()" << restartApp->pid();
                            });
                            restartApp->start(QString("cmd.exe /C start ").append(args.at(0)),QStringList("force"));
                        }
                        */
                    reply->deleteLater();
                });
            });
            qDebug() << "start watchTimer 3000";
            watchTimer->start(3000);
        });
        socket->connectToHost(QHostAddress::LocalHost, rpcPort);
        if (socket->waitForConnected(4000)){
            qDebug() << "watchdog connected" << socket;
            return true;
        }else{
            socket->disconnect();
            socket->abort();
            socket->deleteLater();
        }
        return false;
    }
public slots:
    void start()
    {
        //qDebug() << "Run copy application as watchdog";
        QProcess *watchdogProcess = new QProcess;
        QObject::connect(watchdogProcess,&QProcess::errorOccurred,[](QProcess::ProcessError error){
            qDebug() << "watchdogProcess errorOccurred"<<error;
        });
        watchdogProcess->startDetached(qApp->applicationFilePath().at(0),QStringList("watchdog"));
        /*QObject::connect(&a,&QApplication::aboutToQuit, [watchdogProcess](){
            qDebug() << "QApplication aboutToQuit";
            watchdogProcess->terminate();
        });*/
    }

private:

};

#endif // WATCHDOG_H
