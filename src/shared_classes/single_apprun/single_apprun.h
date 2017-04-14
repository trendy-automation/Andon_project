#ifndef SINGLE_APPRUN_H
#define SINGLE_APPRUN_H

#include <QObject>
#include <QMessageBox>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <QProcess>

class SingleAppRun: public QObject
{
    Q_OBJECT
public:
    SingleAppRun(bool forseStart=false,QObject *parent=0)
        : QObject(parent),
          shmem(new QSharedMemory(parent))
    {
        QString pid= QString::number(qApp->applicationPid());
        if(isRunning(QString("<ANDON %1 VER%2>").arg(APP_NAME).arg(APP_VER),
                     QString("<ANDON %1 VER%2 RUNNING>").arg(APP_NAME).arg(APP_VER), pid)) {
            qDebug() << QString("%1 is already running!").arg(APP_NAME);
            int doTerminate;
            if(forseStart)
                doTerminate=QMessageBox::Yes;
            else
                doTerminate=QMessageBox::question(new QWidget, QString("%1 is already running").arg(APP_NAME),
                                             "Terminate concurent application?");
            //QString old_pid;
            switch (doTerminate) {
            case QMessageBox::Yes:
            {
                QProcess * processKill = new QProcess;
                shmem->lock();
                QString *old_pid = new QString(QString::number(qApp->applicationPid()));
                //old_pid = QString::number(qApp->applicationPid());
                memcpy((char*)shmem->data(), (char *)old_pid->toLatin1().data(),
                        qMin( shmem->size(), old_pid->size()));
                shmem->unlock();
                QString command = QString("taskkill /t /PID %1").arg(pid); // /f
                qDebug()<<command;
                processKill->startDetached(command);
                break;
            }
            case QMessageBox::No:
                qApp->quit();
                break;
            default:
                qDebug()<<"doTerminate"<<doTerminate;
                qApp->quit();
                break;
            }
        }
    }

private:
    bool isRunning(const QString &semaphore, const QString &memory, QString &pid)
    {
        QSystemSemaphore sema(semaphore, 1);
        sema.acquire();
        {
            QSharedMemory shmem2(memory);
            shmem2.attach();
        }
        shmem->setKey(memory);
        if (shmem->attach()){
            shmem->lock();
            pid = QString((char*)shmem->data());
            shmem->unlock();
            return true;
        }
        shmem->create(pid.size());
        shmem->lock();
        memcpy((char*)shmem->data(), (char *)pid.toLatin1().data(), pid.size());
        shmem->unlock();
        sema.release();
        return false;
    }
    QSharedMemory *shmem;
};

#endif // SINGLE_APPRUN_H
