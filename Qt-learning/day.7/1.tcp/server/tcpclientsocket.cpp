#include "tcpserver.h"

TcpClientSocket::TcpClientSocket( QObject *parent)
{  
    connect(this, SIGNAL(readyRead()),this, SLOT(dataReceived()));	 
    connect(this, SIGNAL(disconnected()),this, SLOT(slotDisconnected()));
}

TcpClientSocket::~TcpClientSocket()
{
}
                                                                             
void TcpClientSocket::dataReceived()
{
    QString msg = "";
    while (bytesAvailable()>0)
    {
        const int length=bytesAvailable();
        char *buf = new char[length+1];

        int nr=0;
        while(nr < length)
        {
            nr = read(buf, length);
            msg+=buf;
        }
        delete buf;

        emit updateClients(msg,length);
     }
}

void TcpClientSocket::slotDisconnected()
{
	emit disconnected(this->socketDescriptor ());
}
