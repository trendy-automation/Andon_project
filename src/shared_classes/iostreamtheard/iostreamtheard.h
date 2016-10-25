#ifndef IOSTREAMTHEARD_H
#define IOSTREAMTHEARD_H
#include <iostream>
//#include <QDebug>
#include <QTextStream>
//#include <QMutex>
#include <QThread>

class ioStreamThread : public QThread
{
    Q_OBJECT
public:
    ioStreamThread(QObject *parent=0)
        : QThread(parent)
    {
    }

signals:
    void inputReceived(const QString &text);
protected:
    virtual void run(void)
    {
        QTextStream output(stdout);
        output<<"for refresh client interface, input \"renew [Client IP]\" or \"renew ALL\"\n";
        output.flush();
        QTextStream input(stdin);
        QString newtext;
        for(;;)
        {
            newtext=input.readLine();
            emit inputReceived(newtext);
        }
    }

};

#endif // IOSTREAMTHEARD_H
