#include <QtNetwork>
#include <QtDebug>

#include "server.h"

Server::Server(QObject *parent,int port)
    : QTcpServer(parent)
{
    listen(QHostAddress::Any,port);
}

void Server::incomingConnection(int socketDescriptor)
{
    TcpClientSocket *tcpClientSocket = new TcpClientSocket(this);

    connect(tcpClientSocket,
            SIGNAL(updateClients(QString,int)),
            this,
            SLOT(updateClients(QString,int)));

    connect(tcpClientSocket,
            SIGNAL(disconnected(int)),
            this,
            SLOT(slotDisconnected(int)));
	
    tcpClientSocket->setSocketDescriptor(socketDescriptor);
    tcpClientSocketList.append(tcpClientSocket);
}

void Server::updateClients(QString msg,int length)
{
    emit updateServer(msg,length);

    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        QTcpSocket *item=tcpClientSocketList.at(i);
        QByteArray arr;
        arr.append(msg);
        item->write(arr);
    }
}

void Server::slotDisconnected(int descriptor)
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        QTcpSocket *item=tcpClientSocketList.at(i);
        if(item->socketDescriptor ()==descriptor)
        {
            tcpClientSocketList.removeAt(i);
            item->deleteLater();
            return;
        }
    }
    return;
}
