#include <QTcpServer>
#include <QTcpSocket>

#include "qjsonrpcsocket.h"
#include "qjsonrpcabstractserver_p.h"
#include "qjsonrpctcpserver.h"

//#define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4)
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

#include <QDebug>


class QJsonRpcTcpServerPrivate : public QJsonRpcAbstractServerPrivate
{
public:
    QHash<QTcpSocket*, QJsonRpcSocket*> socketLookup;
};

QJsonRpcTcpServer::QJsonRpcTcpServer(QObject *parent)
#if defined(USE_QT_PRIVATE_HEADERS)
    : QTcpServer(*new QJsonRpcTcpServerPrivate, parent)
#else
    : QTcpServer(parent),
      d_ptr(new QJsonRpcTcpServerPrivate)
#endif
{
}

QJsonRpcTcpServer::~QJsonRpcTcpServer()
{
    Q_D(QJsonRpcTcpServer);
    foreach (QTcpSocket *socket, d->socketLookup.keys()) {
        socket->flush();
        socket->deleteLater();
    }
    d->socketLookup.clear();

    foreach (QJsonRpcSocket *client, d->clients)
        client->deleteLater();
    d->clients.clear();
}

bool QJsonRpcTcpServer::addService(QJsonRpcService *service)
{
    if (!QJsonRpcServiceProvider::addService(service))
        return false;

    connect(service, SIGNAL(notifyConnectedClients(QJsonRpcMessage)),
               this, SLOT(notifyConnectedClients(QJsonRpcMessage)));
    connect(service, SIGNAL(notifyConnectedClients(QString,QJsonArray)),
               this, SLOT(notifyConnectedClients(QString,QJsonArray)));
    return true;
}

bool QJsonRpcTcpServer::removeService(QJsonRpcService *service)
{
    if (!QJsonRpcServiceProvider::removeService(service))
        return false;

    disconnect(service, SIGNAL(notifyConnectedClients(QJsonRpcMessage)),
                  this, SLOT(notifyConnectedClients(QJsonRpcMessage)));
    disconnect(service, SIGNAL(notifyConnectedClients(QString,QJsonArray)),
                  this, SLOT(notifyConnectedClients(QString,QJsonArray)));
    return true;
}

int QJsonRpcTcpServer::connectedClientCount() const
{
    Q_D(const QJsonRpcTcpServer);
    return d->clients.size();
}

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
void QJsonRpcTcpServer::incomingConnection(qintptr socketDescriptor)
#else
void QJsonRpcTcpServer::incomingConnection(int socketDescriptor)
#endif
{
    Q_D(QJsonRpcTcpServer);
    QTcpSocket *tcpSocket = new QTcpSocket(this);
    if (!tcpSocket->setSocketDescriptor(socketDescriptor)) {
        qJsonRpcDebug() << Q_FUNC_INFO << "can't set socket descriptor";
        tcpSocket->deleteLater();
        return;
    }



    struct tcp_keepalive {
    u_long onoff;
    u_long keepalivetime;
    u_long keepaliveinterval;
    };

    DWORD dwError = 0L,dwBytes;
    tcp_keepalive pClSock_tcpKeepalive={0}, sReturned = {0};
    pClSock_tcpKeepalive.onoff=1;
    //включить keepalive
    pClSock_tcpKeepalive.keepalivetime=QJSONRPC_ALIVE_TIMEOUT;
    // каждые KE_ALIVE_TIMEOUT милисекунд отсылать пакет
    pClSock_tcpKeepalive.keepaliveinterval=150;
    // ≈сли не пришел ответ выслать через 1.5с повторно
    if (WSAIoctl(socketDescriptor, SIO_KEEPALIVE_VALS, &pClSock_tcpKeepalive,
    sizeof(pClSock_tcpKeepalive), &sReturned, sizeof(sReturned), &dwBytes,
    NULL, NULL) != 0)
    {dwError = WSAGetLastError() ;
    qWarning((char*)dwError); }


    QIODevice *device = qobject_cast<QIODevice*>(tcpSocket);
    QJsonRpcSocket *socket = new QJsonRpcSocket(device, this);
    socket->setProperty("address",tcpSocket->peerAddress().toString()); //my hack 170615
    connect(socket, SIGNAL(messageReceived(QJsonRpcMessage)),
              this, SLOT(_q_processMessage(QJsonRpcMessage)));
    d->clients.append(socket);
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(_q_clientDisconnected()));
    d->socketLookup.insert(tcpSocket, socket);
    QHostAddress curClientIp=tcpSocket->peerAddress(); //my hack 170615
    bool firstSocket=true;
    foreach (QTcpSocket *tcpSocket_, d->socketLookup.keys())
        if (curClientIp==tcpSocket_->peerAddress()) {
            firstSocket=false;
            break;
        }
    if (firstSocket)
        Q_EMIT clientConnected(curClientIp);
}

void QJsonRpcTcpServer::_q_clientDisconnected()
{
    Q_D(QJsonRpcTcpServer);
    QTcpSocket *tcpSocket = static_cast<QTcpSocket*>(sender());
    if (!tcpSocket) {
        qJsonRpcDebug() << Q_FUNC_INFO << "called with invalid socket";
        return;
    }

    if (d->socketLookup.contains(tcpSocket)) {
        QJsonRpcSocket *socket = d->socketLookup.take(tcpSocket);
        d->clients.removeAll(socket);
        socket->deleteLater();
    }

    tcpSocket->deleteLater();
    QHostAddress curClientIp=tcpSocket->peerAddress(); //my hack 170615
    bool lastSocket=true;
    foreach (QTcpSocket *tcpSocket_, d->socketLookup.keys())
        if (curClientIp==tcpSocket_->peerAddress()) {
            lastSocket=false;
            break;
        }
    if (lastSocket)
        Q_EMIT clientDisconnected(curClientIp);
}

void QJsonRpcTcpServer::_q_processMessage(const QJsonRpcMessage &message)
{
    QJsonRpcSocket *socket = static_cast<QJsonRpcSocket*>(sender());
    if (!socket) {
        qJsonRpcDebug() << Q_FUNC_INFO << "called without service socket";
        return;
    }

    processMessage(socket, message);
}

void QJsonRpcTcpServer::notifyConnectedClients(const QJsonRpcMessage &message)
{
    Q_D(QJsonRpcTcpServer);
    d->_q_notifyConnectedClients(message);
}

void QJsonRpcTcpServer::notifyConnectedClients(const QString &method, const QJsonArray &params)
{
    Q_D(QJsonRpcTcpServer);
    d->_q_notifyConnectedClients(method, params);
}

#include "moc_qjsonrpctcpserver.cpp"
