#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H

#include <QTcpSocket>

class TcpClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpClientSocket( QObject *parent=0);
    ~TcpClientSocket();
signals:
	void updateClients(QString,int);
	void disconnected(int);

protected slots: 
    void dataReceived();    
    void slotDisconnected();
};

#endif 
