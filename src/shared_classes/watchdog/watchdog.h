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
        socket = new QTcpSocket;//(this);
        QObject::connect(socket, &QTcpSocket::connected,this,&Watchdog::runClient);
        socket->connectToHost(QHostAddress::LocalHost, rpcPort);
        if (socket->waitForConnected(5000)){
            //qDebug() << "Watchdog waitForConnected OK";
            return true;
        }
        else{
            //qDebug() << "Watchdog connection falied";
            socket->disconnect();
            socket->abort();
            socket->deleteLater();
        }
        //qDebug() << "Watchdog listen return false";
        return false;
    }
public slots:
    static void start()
    {
        //qDebug() << "watchdog start";
        QProcess *watchdogProcess = new QProcess(qApp);
        QObject::connect(watchdogProcess,&QProcess::errorOccurred,[](QProcess::ProcessError error){
            qDebug() << "watchdogProcess errorOccurred"<<error;
        });
        QObject::connect(watchdogProcess,&QProcess::started,[](){
            //qDebug() << "WatchdogProcess started:"<<qApp->applicationFilePath()<<APP_OPTION_WATHCDOG;
        });
        watchdogProcess->setProcessChannelMode(QProcess::QProcess::ForwardedChannels);
        watchdogProcess->start(qApp->applicationFilePath(),QStringList(APP_OPTION_WATHCDOG));
    }
    static void restartAppication()
    {
        qDebug() << "Force restart application(response timeout)!\n\r"
                 <<QString("cmd.exe /C start \"%1 %2\"").arg(qApp->applicationFilePath()).arg(APP_OPTION_FORCE);
        QProcess *restartApp = new QProcess;//(this);
        QObject::connect(restartApp,&QProcess::errorOccurred,[](QProcess::ProcessError error){
            qDebug() << "restartApp errorOccurred!"<<error;
        });
        QObject::connect(restartApp,&QProcess::started,[](){
            qDebug() << "restartApp started";
        });
        //restartApp->setWorkingDirectory(qApp->applicationDirPath());
        //restartApp->startDetached(QString("cmd.exe /C start \"%1 %2\"")
        //.arg(qApp->applicationFilePath()).arg(APP_OPTION_FORCE));
        restartApp->startDetached(qApp->applicationFilePath(),QStringList()<<APP_OPTION_FORCE);
        qApp->quit();
    }
    static void rebootPC(const QString &reason)
    {
        qDebug() << "Force restart application! Reason:" << reason;
        QProcess *shutdownPC = new QProcess;//(this);
        QObject::connect(shutdownPC,&QProcess::errorOccurred,[](QProcess::ProcessError error){
            qDebug() << "rebootPC errorOccurred!"<<error;
        });
        shutdownPC->startDetached("shutdown.exe",QStringList()<<"-r"<<"-f"<<"-t"<<"0");
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
        watchTimer = new QTimer;//(this);
        QObject::connect(watchTimer, &QTimer::timeout, this, &Watchdog::sendReply);
        watchTimer->start(3000);
    }
private slots:
    void sendReply()
    {
        qDebug() << "Watchdog sendReply";
        QJsonRpcServiceReply *reply = m_client->invokeRemoteMethod(aliveMethod);
        QTimer *replayTimer=new QTimer;//(this);
        replayTimer->setSingleShot(true);
        QObject::connect(replayTimer, &QTimer::timeout, this, &Watchdog::restartAppication);
        QObject::connect(replayTimer, &QTimer::timeout, watchTimer, &QTimer::stop);
        replayTimer->start(1000);
        //QObject::connect(reply, &QJsonRpcServiceReply::finished, replayTimer, &QTimer::stop);
        //QObject::connect(reply, &QJsonRpcServiceReply::finished, replayTimer, &QTimer::deleteLater);
        QObject::connect(reply, &QJsonRpcServiceReply::finished, [reply,replayTimer,this] () {
            replayTimer->stop();
            replayTimer->deleteLater();
            //qDebug() << "alive" << reply->response().result().toVariant().toBool();

                if (!reply->response().result().toVariant().isValid())
                    restartAppication();
            reply->deleteLater();
        });
    }
};

#endif // WATCHDOG_H
