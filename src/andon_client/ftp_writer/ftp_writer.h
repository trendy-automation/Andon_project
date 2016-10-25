#ifndef FTP_H
#define FTP_H

#include <QObject>
#include <QAbstractSocket>
#include <QString>
#include <QTimer>
#include <QBuffer>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>

#ifndef CHAR_CR
#define CHAR_CR QString("\r\n")
#endif

//class QTcpSocket;
//class QTcpServer;
//class QFile;
class Ftp : public QObject
{
     Q_OBJECT
public:
    enum Mode {PASV,PORT};
    enum Type {BINARY='I',ASCII='A',EBCDIC='E',LOCAL='L'};
    enum CMD{CMD_PUT,CMD_GET,CMD_LIST,CMD_OTHER};

public:
    explicit Ftp(const QString &ip, quint16 port, const QString &username, const QString &passwd);
    ~Ftp();
    bool connectToHost(QString ip,quint16 port);
    void login(QString username,QString passwd);
    void mode(Mode transfer_mode){m_mode=transfer_mode;}
    void type(Type transfer_type){m_type=transfer_type;}
    void get(QString local_filename,QString remote_filename,qint64 offset=0);
    qint64 fileSize(QString remote_filename);
    void list(QString remote_dir);
    void rawCommand(QString cmd);
    bool connectStatus(){return b_isConnected;}
    bool loginStatus(){return b_isLogined;}
    void setLogin(const QString &username, const QString &passwd);

private:

    void setTransferProperty();
    bool addDataChannel();

private:
    QString str_ip;
    quint16 n_port;
    quint32 n_taskId;
    QString str_username;
    QString str_password;
    Mode m_mode;
    Type m_type;
    CMD m_cmdType;

    bool b_isConnected;
    bool b_isLogined;
    bool b_isBusy;
    bool b_stop;

    QByteArray m_data;
    QTcpSocket *p_cmdSocket;
    QTcpSocket *p_dataSocket;
    QTcpServer *p_listener;

    QFile *p_file;
    qint64 n_transferValue;
    qint64 n_transferTotal;
    qint64 n_remoteFileSize;

//    QTimer *p_connectTimer;
//    QTimer *p_fileTimer;
signals:
    void loginSuccess();
    void execCmdResult(QString result);
    void transferDataProgress(qint64 transfer_size,qint64 total_size);
    void transferFinished(quint32 taskId);
    void error(quint32 taskId, int code,QString desc);
    void remoteDirInfo(QStringList dirInfo);
    void logout();
    void newTask(QBuffer *buffer, const QString &remote_filename, quint32 taskId);

public slots:
    void login();
    void connectError(QAbstractSocket::SocketError code);
    void stopTransfer(){b_stop=true;}
    void putFile(QBuffer *buffer, const QString &remote_filename, quint32 taskId);
    void startConnecting(int interval=60000, int delay=0);
    bool put(QString local_filename, QString remote_filename, qint64 offset=0, bool is_append=false);

private slots:
    void readCmdResult();
    void getPORTSocket();
    void writeData();
    void readData();
    void readDataFinished();
    void transferData();
    void readDirInfo();
    void clearDataSocket();
//    void addFileTask(QBuffer *buffer, const QString &remote_filename, quint32 taskId)
//        {qDebug()<<"addFileTask"<<remote_filename<<taskId;
//         emit newTask(buffer, remote_filename, taskId);}
};

#endif // FTP_H
