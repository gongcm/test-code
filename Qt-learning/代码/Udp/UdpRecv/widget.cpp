#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    udpSocket = new QUdpSocket(this);
    bool ok = udpSocket->bind(QHostAddress("192.168.1.169"),8888);
    if(!ok){
        qDebug() << "fail to bind : " << udpSocket->errorString();
    }

    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(recvData()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::recvData()
{
    while(udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(),datagram.size(),&peerIp,&peerPort);

        QString text = QString("[%1,%2] : %3\n").arg(peerIp.toString()).arg(peerPort).arg(QString(datagram));
        ui->displayTextEdit->insertPlainText(text);
    }
}

void Widget::on_sendButton_clicked()
{
    QByteArray datagram = ui->inputTextEdit->toPlainText().toUtf8();
    udpSocket->writeDatagram(datagram,peerIp,peerPort);
    ui->inputTextEdit->clear();
}
