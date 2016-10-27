#include "ftp.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QHostAddress>
#include <QFileInfo>
#include <QThread>
#include <QDataStream>

Ftp::Ftp(const QString &ip, quint16 port, const QString &username, const QString &passwd):QObject(0),b_isConnected(false)
{
    p_cmdSocket=0;
    p_listener=0;
    p_file=0;
    p_dataSocket=NULL;
    m_mode=Ftp::PORT;
    m_type=Ftp::BINARY;
    m_cmdType=Ftp::CMD_OTHER;
    n_transferValue=0;
    n_transferTotal=0;
    n_remoteFileSize=0;
    b_stop=false;
    str_ip=ip;
    n_port=port;
    str_username=username;
    str_password=passwd;
    b_isBusy=0;
    p_connectTimer=0;

    QObject::connect(this,&Ftp::transferFinished,[this](){b_isBusy=0;});


    QThread *ftpThread=new QThread;
    this->moveToThread(ftpThread);
    QObject::connect(ftpThread,&QThread::started,[this](){startConnecting(1);});
    QObject::connect(ftpThread, &QThread::finished, ftpThread, &QThread::deleteLater);
    ftpThread->start();

//    if(autoConnect)
//        startConnecting();
}

Ftp::~Ftp()
{
    p_cmdSocket->deleteLater();
    p_listener->deleteLater();
    p_file->deleteLater();
    p_connectTimer->deleteLater();
}

void Ftp::setTransferProperty()
{
    QString cmd="TYPE "+QString(m_type)+CHAR_CR;
    p_cmdSocket->write(cmd.toLatin1());
}

void Ftp::login(QString username,QString passwd)
{

    //connect break!
    if(!b_isConnected)
        return;
    qDebug()<<"login"<<this->thread()<<p_cmdSocket->thread();
    str_username=username;
    str_password=passwd;

    QString cmd="USER "+username+CHAR_CR+"PASS "+passwd+CHAR_CR;

    p_cmdSocket->write(cmd.toLatin1());

    //prepare receive all response data
    QObject::connect(p_cmdSocket,&QTcpSocket::readyRead,this,&Ftp::readCmdResult,Qt::UniqueConnection);
    //set global transfer property
    setTransferProperty();
    QObject::connect(p_listener,&QTcpServer::newConnection,this,&Ftp::getPORTSocket);
}


void Ftp::setLogin(const QString &username, const QString &passwd)
{
    str_username=username;
    str_password=passwd;
}

void Ftp::login()
{
    //try to connect
    if(p_cmdSocket)
        if(p_cmdSocket->state()==QAbstractSocket::UnconnectedState)
            connectToHost(str_ip,n_port);
//            startConnecting();

    if(!b_isConnected)
        return;

    login(str_username,str_password);
}

bool Ftp::connectToHost(QString ip,quint16 port)
{
    qDebug()<<"connectToHost";
    if(!p_file)
        p_file=new QFile;
    if(!p_cmdSocket)
        p_cmdSocket=new QTcpSocket;
    if(!p_listener)
        p_listener=new QTcpServer;

    p_cmdSocket->connectToHost(ip,port);
    QObject::connect(p_cmdSocket,static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>
                     (&QAbstractSocket::error),this,&Ftp::connectError);

    b_isConnected=true;
    //block way run
    if(!p_cmdSocket->waitForConnected())
        b_isConnected=false;
    qDebug()<<"b_isConnected"<<b_isConnected;
    if(b_isConnected && b_autoLogin)
        login(str_username,str_password);
    return b_isConnected;
}

void Ftp::startConnecting(bool autoLogin, int interval, int delay)
{
//QString ip,quint16 port,
//    str_ip=ip;
//    n_port=port;
    b_autoLogin=autoLogin;
    qDebug()<<"startConnecting";
    if(!connectToHost(str_ip,n_port))
        if(!p_connectTimer){
            qDebug()<<"p_connectTimer = new QTimer";
            p_connectTimer=new QTimer;
            QObject::connect(p_connectTimer,&QTimer::timeout,[this, interval](){
                    if (connectToHost(str_ip,n_port)) {
                        p_connectTimer->stop();
                        p_connectTimer->deleteLater();
                        p_connectTimer=0;
                    } else
                        p_connectTimer->start(interval);
            });
            p_connectTimer->start(delay);
        }
}

void Ftp::addDataChannel()
{
    QString cmd="";
    if(m_mode==Ftp::PORT){
        QHostAddress host(str_ip);
        if(p_listener->isListening())
            p_listener->close();
        if(!p_listener->listen(host,0)){
            emit error(9,"fail for build data channel!");
            return;
        }

        quint16 port=p_listener->serverPort();
        quint32 ipVal=host.toIPv4Address();
        QString address=QString::number((ipVal&0xff000000)>>24)+QChar(',')+QString::number((ipVal&0xff0000)>>16)+QChar(',')+QString::number((ipVal&0xff00)>>8)+QChar(',')+QString::number(ipVal&0xff);
        address+=QChar(',')+QString::number((port&0xff00)>>8)+QChar(',')+QString::number(port&0xff);
        cmd="PORT "+address+CHAR_CR;
        p_cmdSocket->write(cmd.toLatin1());
    }
    else{
        cmd="PASV"+CHAR_CR;
        p_cmdSocket->write(cmd.toLatin1());
    }

}

void Ftp::getPORTSocket()
{
    p_dataSocket=p_listener->nextPendingConnection();
    //start transfer data
    transferData();
}

void Ftp::put(QString local_filename,QString remote_filename,qint64 offset,bool is_append)
{
    if(!b_isLogined || p_dataSocket!=NULL || b_isBusy)
//    if(!b_isLogined)
        return;
    b_isBusy=1;
    rawCommand("noop\r\n");
    rawCommand("CWD /test/\r\n");
    rawCommand("TYPE I\r\n");
    rawCommand("PASV\r\n");

    //qDebug()<<"Ftp::put"<<remote_filename;
//    if(p_dataSocket!=NULL)
//        clearDataSocket();
//        p_dataSocket = p_listener->nextPendingConnection();

    QFileInfo info(local_filename);
    if(!info.exists() || !info.isReadable()){
        emit error(11,"can`t read local file!");
        return;
    }

    if(p_file->isOpen())
        p_file->close();
//    if(!local_filename.isEmpty())
        p_file->setFileName(local_filename);
    if(!p_file->open(QIODevice::ReadOnly)){
        emit error(11,p_file->errorString());
        return;
    }

    qDebug()<<"info.size()"<<info.size();

    QString cmd="ALLO "+QString::number(info.size()-offset)+CHAR_CR;
    if(is_append)
        cmd+="APPE "+remote_filename+CHAR_CR;
    if(offset>0)
        cmd+="REST "+QString::number(offset)+CHAR_CR;
//    qDebug()<<"remote_filename"<<remote_filename;
    cmd+="STOR "+remote_filename+CHAR_CR;

    n_transferValue=offset;
    n_transferTotal=info.size();
    if(n_transferValue>=n_transferTotal){
        emit transferFinished();
        return;
    }

    //build data channel
    addDataChannel();
    b_stop=false;

    p_file->seek(offset);
    m_cmdType=Ftp::CMD_PUT;

    p_cmdSocket->write(cmd.toLatin1());
}

void Ftp::get(QString local_filename,QString remote_filename,qint64 offset)
{
    if(!b_isLogined || p_dataSocket!=NULL)
        return;

    qint64 remoteFileSize=fileSize(remote_filename);
    if(remoteFileSize<=0)
        return;


    if(p_file->isOpen())
        p_file->close();

    p_file->setFileName(local_filename);
    if(!p_file->open(QIODevice::WriteOnly)){
        emit error(11,p_file->errorString());
        return;
    }

    QString cmd="";
    if(offset>0)
        cmd+="REST "+QString::number(offset)+CHAR_CR;

    cmd+="RETR "+remote_filename+CHAR_CR;

    n_transferValue=offset;
    n_transferTotal=remoteFileSize;
    if(n_transferValue>=n_transferTotal){
        emit transferFinished();
        return;
    }

    //build data channel
    addDataChannel();

    b_stop=false;
    m_cmdType=Ftp::CMD_GET;
    p_cmdSocket->write(cmd.toLatin1());
}

void Ftp::list(QString remote_dir)
{
    if(!b_isLogined || p_dataSocket!=NULL)
        return;

    addDataChannel();

    QString cmd="LIST "+remote_dir+CHAR_CR;

    m_cmdType=Ftp::CMD_LIST;
    p_cmdSocket->write(cmd.toLatin1());
}

void Ftp::rawCommand(QString cmd)
{
    if(!b_isLogined || cmd.isEmpty())
        return;
    m_cmdType=Ftp::CMD_OTHER;
    p_cmdSocket->write(cmd.toLatin1());
}

qint64 Ftp::fileSize(QString remote_filename)
{
    if(!b_isLogined || remote_filename.length()<=0)
        return 0;

    n_remoteFileSize=0;
    QString cmd="SIZE "+remote_filename+CHAR_CR;
    if(p_cmdSocket->write(cmd.toLatin1())<=0 || !p_cmdSocket->waitForReadyRead())
        return 0;

    return n_remoteFileSize;
}

void Ftp::connectError(QAbstractSocket::SocketError code)
{
    switch(code){
    case QTcpSocket::ConnectionRefusedError:
        emit error(0,"connect resfuse error!");
        break;
    case QTcpSocket::RemoteHostClosedError:
        emit error(1,"remote host closed!");
        break;
    case QTcpSocket::HostNotFoundError:
        emit error(2,"host not found!");
        break;
    case QTcpSocket::SocketTimeoutError:
        emit error(3,"connect timeout!");
        break;
    case QTcpSocket::NetworkError:
        emit error(4,"network error!");
        break;
    default:
        emit error(code,"unkown error,please check tcp socket!");
    }

    b_isConnected=false;
}

void Ftp::readCmdResult()
{
    QByteArray data;

    while((data=p_cmdSocket->readLine()).length()>0){
        QString result=QString(data);
        QRegExp regexp("^\\d{3}\.+");
//        qDebug()<<"readCmdResult result"<<result<<"p_dataSocket "
//                <<((p_dataSocket!=NULL && p_dataSocket!=0x0)?"OK":"is NULL");

        if(!regexp.exactMatch(result))
            continue;

        QStringList strlist=result.split(' ');
        bool toInt=false;
        int code=strlist.first().toInt(&toInt);
        if(!toInt)
            continue;

        switch(code){
            case 200://Type set to I
                    if(this->property("loginSuccess").toBool()){
                        this->setProperty("loginSuccess",0);
                        emit loginSuccess();
                    }
                break;
            case 230://login success
                    this->setProperty("loginSuccess",1);
//                    emit loginSuccess();
                b_isLogined=true;
                break;
            case 227:{ //build pasv data channel
                    QRegExp regexp("\(?:(\\d{1,3}),(\\d{1,3}),(\\d{1,3}),(\\d{1,3}),(\\d{1,3}),(\\d{1,3})\)");
                    QString ip;quint16 port;
                    if(regexp.indexIn(result)!=-1){
                        ip=regexp.cap(1)+"."+regexp.cap(2)+"."+regexp.cap(3)+"."+regexp.cap(4);
                        port=(regexp.cap(5).toUInt()<<8)+regexp.cap(6).toUInt();
                    }

                    //add new data connect,(old connect ignore)
                    p_dataSocket=new QTcpSocket;
                    p_dataSocket->connectToHost(QHostAddress(ip),port);
                    if(!p_dataSocket->waitForConnected()){
                        emit error(9,"fial for build data channel!");
                        return;
                    }

                    //start transfer data
                    transferData();
                }
                break;
           case 213:{
                bool toInt=false;
                qint64 size=strlist.last().toLongLong(&toInt);
                if(toInt && size>0)
                    n_remoteFileSize=size;
                }break;
            case 421://FTP timeout
                b_isLogined=false;
                emit logout();
                break;
            case 530://ftp password error
                b_isLogined=false;
                p_cmdSocket->disconnectFromHost();
                p_cmdSocket->close();
                emit error(5,"Ftp login  error!");
            default:
                break;
        }

        emit execCmdResult(result);
    }
}

void Ftp::transferData()
{
    if(p_dataSocket==NULL)
        return;

    switch(m_cmdType){
    case Ftp::CMD_GET:
        //prepare to download
        QObject::connect(p_dataSocket,&QTcpSocket::readyRead,this,&Ftp::readData);
        QObject::connect(p_dataSocket,&QTcpSocket::readChannelFinished,this,&Ftp::readDataFinished);
        break;
    case Ftp::CMD_PUT:
        //prepare to upload
        QObject::connect(p_dataSocket,&QTcpSocket::bytesWritten,this,&Ftp::writeData);
        writeData();
        break;
    case Ftp::CMD_LIST:
        //read dir info
        QObject::connect(p_dataSocket,&QTcpSocket::readyRead,this,&Ftp::readDirInfo);
        break;
    default:
        break;
    }

    //clear data socket connect
    QObject::connect(p_dataSocket,&QTcpSocket::disconnected,this, &Ftp::clearDataSocket);
}

void Ftp::clearDataSocket()
{
    if(p_dataSocket!=NULL && p_dataSocket!=0x0){
        p_dataSocket->close();
        p_dataSocket->deleteLater();
    }
    p_dataSocket=NULL;
}

void Ftp::readDirInfo()
{
    if(m_cmdType!=Ftp::CMD_LIST)
        return;

    QStringList dirInfo;
    while(p_dataSocket->bytesAvailable()>0){
        QString info=QString(p_dataSocket->readLine());
        if(info.length()<=0)
            continue;

        dirInfo.append(info);
    }

    emit remoteDirInfo(dirInfo);
    clearDataSocket();
}

void Ftp::readDataFinished()
{
    p_file->close();

    clearDataSocket();
    emit transferFinished();
    return;
}

void Ftp::readData()
{
    if(m_cmdType!=Ftp::CMD_GET)
        return;

    int bufsize=8*1024;//write size 8KB
    p_dataSocket->setReadBufferSize(bufsize*16);//socket buffer size 128KB

    while(p_dataSocket->bytesAvailable()>0){
        QByteArray data=p_dataSocket->read(bufsize);
        if(b_stop){
            readDataFinished();
            return;
        }

        qint64 bytesWrite=p_file->write(data);
        if(bytesWrite==-1){
            emit error(12,"fail for write to file!");
            p_file->close();
            clearDataSocket();
            return;
        }

        n_transferValue+=bytesWrite;
        emit transferDataProgress(n_transferValue,n_transferTotal);
    }
}

void Ftp::writeData()
{
    if(!p_file->isOpen())
        return;

    int bufsize=p_file->size();
    char buffer[bufsize];

    QDataStream in(p_file);
//    in.setVersion(QDataStream::Qt_5_6);
    int read=in.readRawData(buffer,bufsize);
    //qint64 read=p_file->read(buffer,bufsize);
    if(read==-1){
        emit error(11,"can't read file!");
        p_file->close();
        clearDataSocket();
        return;
    }

    if(read==0 || b_stop){
        p_file->close();
        if(p_dataSocket!=NULL)
            p_dataSocket->disconnectFromHost();
        emit transferFinished();
        return;
    }

    if(read>0){
        qint64 bytesWrite=p_dataSocket->write(buffer,bufsize);
        if(bytesWrite==0){
            clearDataSocket();
            emit error(13,"fail for write to server!");
            return;
        }

        n_transferValue+=bytesWrite;
        emit transferDataProgress(n_transferValue,n_transferTotal);
    }
}


void Ftp::putFile(QBuffer *buffer, const QString &remote_filename)
{
    //TODO signal with result

//    if(!b_isConnected)
//        if(!connectToHost(str_ip,n_port)){
//            startConnecting(1);
//        }
//    if(!b_isLogined)
//        login();
    qDebug()<<"putFile";
    const QString &temp_filename("tmp_ftp");
    if(p_file){
        p_file->setFileName(temp_filename);
        p_file->open(QIODevice::WriteOnly);
        qDebug()<<"buffer->size()"<<buffer->size();
        p_file->write(buffer->data(),buffer->size());
        p_file->close();
        put(temp_filename,remote_filename);
    }
}
