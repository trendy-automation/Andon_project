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
    bool listen(int rpcPort, const QString &method)
    {
        aliveMethod=method;
        socket = new QTcpSocket(this);
        QObject::connect(socket, &QTcpSocket::connected,this,&Watchdog::runClient);
        socket->connectToHost(QHostAddress::LocalHost, rpcPort);
        if (socket->waitForConnected(4000))
            return true;
        else{
            qDebug() << "Watchdog connection falied";
            socket->disconnect();
            socket->abort();
            socket->deleteLater();
        }
        return false;
    }
public slots:
    static void start()
    {
        QProcess *watchdogProcess = new QProcess;
        QObject::connect(watchdogProcess,&QProcess::errorOccurred,[](QProcess::ProcessError error){
            qDebug() << "watchdogProcess errorOccurred"<<error;
        });
        watchdogProcess->startDetached(qApp->applicationFilePath().at(0),QStringList("watchdog"));
    }
    static void restartAppication()
    {
        qDebug() << "Force restart application(response timeout)!";
        QProcess *restartApp = new QProcess;
        QObject::connect(restartApp,&QProcess::errorOccurred,[](QProcess::ProcessError error){
            qDebug() << "restartApp errorOccurred!"<<error;
        });
        restartApp->start("cmd.exe",QStringList()<<"/C"<<"start"<<qApp->applicationFilePath().at(0)<<"force");
        qApp->quit();
    }
    static void rebootPC(const QString &reason)
    {
        qDebug() << "Force restart application! Reason:" << reason;
        QProcess *shutdownPC = new QProcess;
        QObject::connect(shutdownPC,&QProcess::errorOccurred,[](QProcess::ProcessError error){
            qDebug() << "rebootPC errorOccurred!"<<error;
        });
        shutdownPC->start("shutdown.exe",QStringList()<<"-r"<<"-f"<<"-t"<<"0");
        qApp->quit();
    }

private:
    QTimer *watchTimer;
    QString aliveMethod;
    QJsonRpcSocket *m_client;
    QTcpSocket *socket;

    void runClient()
    {
        qDebug() << "Application watchdog started";
        m_client = new QJsonRpcSocket(socket);
        watchTimer = new QTimer(this);
        QObject::connect(watchTimer, &QTimer::timeout, this, &Watchdog::sendReply);
        watchTimer->start(3000);
    }
private slots:
    void sendReply()
    {
        QJsonRpcServiceReply *reply = m_client->invokeRemoteMethod(aliveMethod);
        QTimer *replayTimer=new QTimer(this);
        replayTimer->setSingleShot(true);
        QObject::connect(replayTimer, &QTimer::timeout, this, &Watchdog::restartAppication);
        QObject::connect(replayTimer, &QTimer::timeout, watchTimer, &QTimer::stop);
        replayTimer->start(1000);
        QObject::connect(reply, &QJsonRpcServiceReply::finished, replayTimer, &QTimer::stop);
        /*QObject::connect(reply, &QJsonRpcServiceReply::finished, [reply,replayTimer,watchTimer] () {
            replayTimer->stop();
            replayTimer->deleteLater();
            //qDebug() << "alive" << reply->response().result().toVariant().toBool();

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

            reply->deleteLater();
        });*/
    }
};

#endif // WATCHDOG_H
