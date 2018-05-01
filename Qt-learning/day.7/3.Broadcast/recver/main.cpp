#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QScreen>
#include <QPixmap>
#include <QTimer>
#include <QFile>
#include <QDesktopWidget>
#include <QDebug>

#include <QUdpSocket>
#include <QHostAddress>
#include <QPushButton>
#include <QHBoxLayout>

#include <QBuffer>

#define W 640
#define H 480

#define PORT 600000

class Udprecver : public QWidget
{
    Q_OBJECT

public:
    explicit Udprecver(QWidget *parent = 0);
    ~Udprecver();

private slots:
    void slotprnscreen();
    void paintEvent(QPaintEvent * event);

private:
    QPixmap pix;
    QUdpSocket *socket;
};

Udprecver::Udprecver(QWidget *parent):
    QWidget(parent)
{
    this->setMinimumSize(W,H);

    socket = new QUdpSocket(this);
    socket->bind(PORT, QUdpSocket::ShareAddress);
    QObject::connect(socket, SIGNAL(readyRead()),this, SLOT(slotprnscreen()));
}

Udprecver::~Udprecver()
{

}

void Udprecver::slotprnscreen()
{
    int size;
    if(-1 == socket->readDatagram((char *)(&size), sizeof(int))){
        qDebug() << "[TRACE]" << socket->errorString() << endl;
        qDebug("[TRACE-%d]%s:%s:%d\n", size, __FILE__, __func__, __LINE__);
        return;
    }

    qDebug("Size: %d\n", size);

    QByteArray bytes(size, 0);
    char *p = bytes.data();

#define PACKMAX 1024
    while(1){
        if(socket->waitForReadyRead(1)){
            if(size > PACKMAX){
                if(PACKMAX != socket->readDatagram(p, PACKMAX)){
                    this->setWindowTitle("1"+socket->errorString());
                    //qDebug() << "[TRACE]" << socket->errorString() << endl;
                    //qDebug("[TRACE-%d]%s:%s:%d\n", size, __FILE__, __func__, __LINE__);
                    break;
                }
            }else{
                if(size != socket->readDatagram(p, size)){
                    this->setWindowTitle("2"+socket->errorString());
                    //qDebug() << "[TRACE]" << socket->errorString() << endl;
                    //qDebug("[TRACE-%d]%s:%s:%d\n", size, __FILE__, __func__, __LINE__);
                }
                break;
            }

            size -= PACKMAX;
            p += PACKMAX;
        }else
            qApp->processEvents();
    }

    pix.loadFromData(bytes);
    this->update();
}

void Udprecver::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    if(pix.size().width()){
        qDebug() << "--------------------" << endl;
        p.drawPixmap(0,0,this->width(),this->height(),pix);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Udprecver recver;
    recver.show();

    return a.exec();
}

#include "main.moc"
