#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void recvData();

private slots:
    void on_sendButton_clicked();

private:
    Ui::Widget *ui;
    QUdpSocket *udpSocket;
    QHostAddress peerIp;
    quint16 peerPort;
};

#endif // WIDGET_H
