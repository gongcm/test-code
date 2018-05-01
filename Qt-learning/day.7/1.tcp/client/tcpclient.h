#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <QTcpSocket>
#include <QHostAddress>
#include <QString>

class TcpClient : public QDialog
{
    Q_OBJECT
public:
    explicit TcpClient(QWidget *parent=0, Qt::WindowFlags f=0);
    ~TcpClient();

protected slots:
    void slotEnter();
    void slotConnected();
    void slotDisconnected();
    void slotSend();
    void dataReceived();

private:
	QListWidget *ListWidgetContent;
	QLineEdit* LineEditSend;
	QPushButton* PushButtonSend;
    QLabel* LabelUser;
    QLineEdit* LineEditUser;
    QLabel* LabelServerIP;
    QLineEdit* LineEditServerIP;    	
    QLabel* LabelPort;
    QLineEdit* LineEditPort;
    QPushButton* PushButtonEnter;
    
    QString userName;
    int port;
    QHostAddress *serverIP;
    QTcpSocket *tcpSocket;
};


#endif 
