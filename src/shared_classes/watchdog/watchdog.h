#ifndef WATCHDOG_H
#define WATCHDOG_H
#include <QApplication>
#include <QProcess>
#include <QTcpSocket>
#include <QTimer>
#include <QRegExp>
#include <QStringList>

#include "qjsonrpcsocket.h"
#include "qjsonrpcservice.h"
#include "qjsonrpcservicereply.h"


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
        qDebug() << "watchdog start";
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
    void restartAppication(const QString &reason="", bool cmdFlag=false)
    {
        QString appPath(shieldPath(qApp->applicationFilePath()));
        //QString appPath(qApp->applicationFilePath());
        if(cmdFlag) //Windows 10
            appPath = QString("cmd.exe /C start %1 %2").arg(appPath).arg(APP_OPTION_FORCE);
        else        //Windows 7
            appPath = QString("%1 %2").arg(appPath).arg(APP_OPTION_FORCE);
        qDebug() << "Force restart application!"<<reason<<appPath;
        watchTimer->stop();
        QProcess *restartApp = new QProcess(0);//(this);
        QObject::connect(restartApp,&QProcess::errorOccurred,[](QProcess::ProcessError error){
            qDebug() << "restartApp errorOccurred!"<<error;
        });
        QObject::connect(restartApp,&QProcess::started,[](){
            qDebug() << "restartApp started";
        });
        QTimer *restartTimer=new QTimer;
        restartTimer->setSingleShot(true);
        this->setProperty("restarted",false);
        QObject::connect(restartTimer, &QTimer::timeout,[this,cmdFlag](){
            if(!this->property("restarted").toBool())
                restartAppication("Second restartApp started",!cmdFlag);
            this->setProperty("restarted",true);
        });
        restartTimer->start(8000);
        restartApp->startDetached(appPath);
        //qDebug() << QString("%1 %2").arg(appPath).arg(APP_OPTION_FORCE);
        //restartApp->startDetached(QString("cmd.exe /C start \"\" \"%1\" -arg \"%2\"").arg(qApp->applicationFilePath()).arg(APP_OPTION_FORCE));
        //restartApp->startDetached(QString("cmd.exe /C start %1 %2").arg(appPath).arg(APP_OPTION_FORCE));
        //restartApp->startDetached(QString("%1 %2").arg(appPath).arg(APP_OPTION_FORCE));
        qApp->quit();
    }
    static void rebootPC(const QString &reason="")
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
        watchTimer->start(5000);
    }

    QString shieldPath(const QString &anyPath)
    {
        QString resPath(anyPath);
                QRegExp rx("/((\\w+\\s+)+\\w+)/");
                if(rx.indexIn(anyPath)!=-1){
                    QStringList list = rx.capturedTexts();
                    list.removeDuplicates();
                    for(QString &s:list){
                        if(!s.endsWith(" ") && !s.endsWith("/")){
                            resPath.replace(s,QString("\"%1\"").arg(s));
                        }
                    }
                }
                return resPath;
    }

private slots:
    void sendReply()
    {
        //qDebug() << "Watchdog sendReply";
        QJsonRpcServiceReply *reply  = m_client->invokeRemoteMethod(aliveMethod);
        QTimer *replayTimer=new QTimer;
        QObject::connect(reply, &QJsonRpcServiceReply::finished, [reply,replayTimer,this] () {
            replayTimer->stop();
            if (!reply->response().result().toVariant().isValid())
                restartAppication(QString("Alive reply is wrong. %1").arg(reply->response().errorMessage()));
        });
        replayTimer->setSingleShot(true);
        QObject::connect(replayTimer, &QTimer::timeout,[this] () {
            restartAppication("Alive response timeout.");
        });
        replayTimer->start(5000);
    }
    /*
    QVariant ClientRpcUtility::query(const QString &queryText)
    {
        QEventLoop replyWaitLoop;
        QJsonRpcServiceReply * reply = ServerExecute("SQLQuery2Json", QVariantList()<<queryText);
        if (reply){
            QObject::connect(reply, &QJsonRpcServiceReply::finished, &replyWaitLoop, &QEventLoop::quit);
            //TODO signal jsonrpc timeout
            QTimer::singleShot(JSONRPC_REPLY_TIMEOUT, &replyWaitLoop, &QEventLoop::quit);
            replyWaitLoop.exec();
            if (!reply->response().result().toVariant().isValid())
                qDebug() << "invalid response received!!!"
                         << reply->response().errorMessage();
            else
                return reply->response().result().toVariant();
        }
        return QVariant();
    }
    */
};

#endif // WATCHDOG_H
