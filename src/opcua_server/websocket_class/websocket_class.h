#ifndef WEBSOCKET_CLASS_H
#define WEBSOCKET_CLASS_H

//#include <QThread>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QDebug>
#include <QTime>
#include <QTimer>



class WebSocketClass : public QObject //QThread
{
    Q_OBJECT
public:
    WebSocketClass(QObject *parent=0)    : QObject(parent) //QThread(parent)
    {
    }
signals:
    serverStart();
    serverStop();

public slots:
    void svrStart() {
        emit serverStart();}
    void svrStop() {
        emit serverStop();}


};

#endif // WEBSOCKET_CLASS_H
