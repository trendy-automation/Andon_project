#include <QApplication>
#include "ftp.h"
#include <QThread>
#include <QDataStream>
#include <QFile>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    qDebug()<<"QApplication a"<<QThread::currentThread();
    Ftp *ftp=new Ftp("10.208.105.70",21,"FtpUser","andon");

//    QByteArray ba;
//    QBuffer *buffer=new QBuffer(&ba);
//    buffer->open(QIODevice::WriteOnly);
//    QDataStream out(buffer);
//    out << "Литьевая деталь 1 15 штук";

    QBuffer *buffer=new QBuffer;
    buffer->open(QBuffer::WriteOnly);
    buffer->write("Литьевая деталь 1 15 штук");
    buffer->seek(0);


    QObject::connect(ftp, &Ftp::loginSuccess,[ftp,buffer](){
        qDebug()<<"loginSuccess"<<QThread::currentThread()<<buffer->size();
        //ftp->putFile(buffer,"declare.txt");

        const QString &temp_filename("tmp_ftp");
        QFile *p_file = new QFile(temp_filename);
        //p_file->setFileName(temp_filename);
        p_file->open(QIODevice::WriteOnly);
        qDebug()<<"buffer->size()"<<buffer->size();
        p_file->write(buffer->data(),buffer->size());
        p_file->close();
        ftp->put(temp_filename,"declare.txt");

    });

//    QObject::connect(ftp, &Ftp::loginSuccess,[ftp](){
//        qDebug()<<"loginSuccess"<<QThread::currentThread();
//        ftp->put("moc_ftp.cpp","moc_ftp1.cpp");

//    });

//    QObject::connect(ftp, &Ftp::transferFinished,[ftp](){
//        qDebug()<<"transferFinished"<<QThread::currentThread();
//        ftp->put("moc_ftp.cpp","moc_ftp3.cpp");
//        ftp->disconnect();
//    });


    return a.exec();
}
