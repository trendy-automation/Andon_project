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
    SingleAppRun(QObject *parent=0)
        : QObject(parent)
    {
        QString pid= QString::number(qApp->applicationPid());
        if(isRunning(QString("<ANDON %1 VER%2>").arg(APP_NAME).arg(APP_VER),
                     QString("<ANDON %1 VER%2 RUNNING>").arg(APP_NAME).arg(APP_VER), pid)) {
            qDebug() << QString("%1 is already running").arg(APP_NAME);
            int answer=QMessageBox::question(new QWidget, QString("%1 is already running").arg(APP_NAME),
                                         "Terminate concurent application?");
            QString old_pid;
            switch (answer) {
                case QMessageBox::Yes:
                {
                    QProcess * processKill;
                    shmem.lock();
                    old_pid = QString::number(qApp->applicationPid());
                    memcpy( (char*)shmem.data(), (char *)old_pid.toLatin1().data(),
                            qMin( shmem.size(), old_pid.size()));
                    shmem.unlock();
                    QString command = QString("taskkill /t /PID %1").arg(pid); // /f
                    qDebug()<<command;
                    processKill->startDetached(command);
                    break;
                    }
                case QMessageBox::No:
                    qApp->quit();
                    break;
                default:
                    qDebug()<<"answer"<<answer;
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
            shmem.setKey(memory);
            bool isAppRunning=shmem.attach();
            if (isAppRunning) {
                shmem.lock();
                pid = QString((char*)shmem.data());
                shmem.unlock();
            }
            else {
                shmem.create(pid.size());
                shmem.lock();
                memcpy((char*)shmem.data(), (char *)pid.toLatin1().data(), pid.size());
                shmem.unlock();
            }
            sema.release();
            return isAppRunning;
    }

    QSharedMemory shmem;
};

#endif // SINGLE_APPRUN_H
