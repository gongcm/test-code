#include "widget.h"
#include "ui_widget.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress("192.168.1.169"),8888);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(acceptClientConnect()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::acceptClientConnect()
{
    tcpSocket = tcpServer->nextPendingConnection();

    qDebug() << "peer ip : " << tcpSocket->peerAddress().toString();
    qDebug() << "peer ip : " << tcpSocket->peerPort();

    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readClientData()));
}

void Widget::readClientData()
{
    QByteArray data = tcpSocket->readAll();
    QString dataString = QString(data);
    QString peerIp = tcpSocket->peerAddress().toString();
    int     port   = tcpSocket->peerPort();
    QString text = QString("[%1,%2] : %3\n").arg(peerIp).arg(port).arg(dataString);
    ui->textEdit->insertPlainText(text);
}
