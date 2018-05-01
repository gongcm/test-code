#include <QVBoxLayout>
#include <QMessageBox>
#include <QTextCodec>

#include "tcpclient.h"

TcpClient::TcpClient( QWidget *parent, Qt::WindowFlags  f )
    : QDialog( parent, f )
{  
    setWindowTitle(QString("TCP Client"));

    QVBoxLayout *vbMain = new QVBoxLayout( this );

    ListWidgetContent = new QListWidget( this);
    vbMain->addWidget( ListWidgetContent );

    QHBoxLayout *hb1 = new QHBoxLayout();
	  
    LineEditSend = new QLineEdit(this);
    hb1->addWidget( LineEditSend );	

    PushButtonSend = new QPushButton( this);
    PushButtonSend->setText(QString( "Send" ));
    hb1->addWidget( PushButtonSend );

	vbMain->addLayout( hb1 );

    QHBoxLayout *hb2 = new QHBoxLayout( );

    LabelUser = new QLabel( this );
    LabelUser->setText(QString("User Name:"));
    hb2->addWidget( LabelUser );
   
    LineEditUser = new QLineEdit(this);
    LineEditUser->setText(QString("user"));
    hb2->addWidget(LineEditUser);

	QHBoxLayout *hb3 = new QHBoxLayout( );

    LabelServerIP = new QLabel( this );
    LabelServerIP->setText(QString("Server:"));
    hb3->addWidget( LabelServerIP );
   
    LineEditServerIP = new QLineEdit(this);
    LineEditServerIP->setText(QString("127.0.0.1"));
    hb3->addWidget( LineEditServerIP );

    QHBoxLayout *hb4 = new QHBoxLayout( );
    
    LabelPort = new QLabel( this );
    LabelPort->setText(QString("Port:"));
    hb4->addWidget( LabelPort );
   
    LineEditPort = new QLineEdit(this);
    hb4->addWidget( LineEditPort );
    
    vbMain->addLayout(hb2);
    vbMain->addLayout(hb3);
    vbMain->addLayout(hb4);
           
    PushButtonEnter = new QPushButton( this);
    PushButtonEnter->setText(QString("Connect"));
    vbMain->addWidget( PushButtonEnter );

    connect(PushButtonEnter,SIGNAL(clicked()),this,SLOT(slotEnter()));
    connect(PushButtonSend,SIGNAL(clicked()),this,SLOT(slotSend()));	

	serverIP = new QHostAddress();
    port = 9999;
	LineEditPort->setText(QString::number(port)); 

    PushButtonSend->setEnabled(false);
}

TcpClient::~TcpClient()
{
}
                                                                             
void TcpClient::slotEnter()                     
{
    if(PushButtonEnter->text() == QString("Connect")) {
        QString ip=LineEditServerIP->text();
        if(!serverIP->setAddress(ip)){
            QMessageBox::information(this,QString("error"),QString("server ip address error!"));
			return;
		}

        if(LineEditUser->text()==QString("")){
            QMessageBox::information(this,QString("error"),QString("User name error!"));
			return ;
		}	

        userName = LineEditUser->text();
	
		tcpSocket = new QTcpSocket(this);
		connect(tcpSocket,SIGNAL(connected()),this,SLOT(slotConnected()));
		connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
        connect(tcpSocket, SIGNAL(readyRead()),this, SLOT(dataReceived()));

        port = LineEditPort->text().toInt();
        tcpSocket->connectToHost(*serverIP, port);

        PushButtonEnter->setEnabled(false);
    } else {
        QString msg=userName+tr(":Leave Chat Room");
        std::string text = msg.toStdString();
        int len = text.length();
        if(len != tcpSocket->write(text.data(), len)){
	    	return ;
        }

        tcpSocket->disconnectFromHost();
	}	
}

void TcpClient::slotConnected()                     
{
    PushButtonSend->setEnabled(true);

    PushButtonEnter->setEnabled(true);
    PushButtonEnter->setText(QString("Leave"));

    QString msg=userName+QString(":Enter Chat Room");
    std::string text = msg.toStdString();
    int len = text.length();
    tcpSocket->write(text.data(), len);
}

void TcpClient::slotDisconnected()                     
{
	PushButtonSend->setEnabled( false );
    PushButtonEnter->setText(QString("Connect"));
}

void TcpClient::slotSend()                     
{         
    if(LineEditSend->text() == QString("")){
		return ;
	}
	
    QString msg=userName+QString(":")+LineEditSend->text();
    QTextStream out(tcpSocket);
    out << msg;

    LineEditSend->clear();
}

void TcpClient::dataReceived()
{
    while (tcpSocket->bytesAvailable()>0){
        QString msg;
        QTextStream in(tcpSocket);

        in.setCodec(QTextCodec::codecForName("GBK2312"));
        in >> msg;

        ListWidgetContent->addItem (msg);
    }
}
