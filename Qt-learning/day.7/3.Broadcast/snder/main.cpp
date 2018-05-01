#include <QApplication>
#include <QDialog>
#include <QLabel>
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
#include <QTime>

#define PORT 600000

class Udpsender : public QWidget
{
    Q_OBJECT

public:
    explicit Udpsender(QWidget *parent = 0);
    ~Udpsender();

private slots:
    void slotsavescreen();
    void slotstart();

private:
    void sndpixmap(QPixmap &pix);
    void delay(int ms);
private:
    int counter;
    QTimer *tm;
    QLabel *label;
    QPushButton *btn;
    QHBoxLayout *layout;

    QUdpSocket *socket;
};

Udpsender::Udpsender(QWidget *parent):
    QWidget(parent)
{
    counter = 0;
    label = new QLabel("...", this);
    label->setAlignment(Qt::AlignCenter);

    btn = new QPushButton("&Start", this);
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(slotstart()));

    layout = new QHBoxLayout(this);
    layout->addWidget(label);
    layout->addStretch();
    layout->addWidget(btn);

#define W 320
#define H 60
    this->setMinimumSize(W,H);
    this->setMaximumSize(W,H);

    tm = new QTimer(this);
    QObject::connect(tm, SIGNAL(timeout()), this, SLOT(slotsavescreen()));

    socket = new QUdpSocket(this);
}

Udpsender::~Udpsender()
{

}

#define SNDDELAY 10
void Udpsender::delay(int ms)
{
    QTime end = QTime::currentTime().addMSecs(ms);

    while(QTime::currentTime() < end)
        QApplication::processEvents();
}

void Udpsender::sndpixmap(QPixmap &pix)
{
#if 0
    pix = pix.scaled(800,600);
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    QString text;
    if(pix.save(&buffer, "JPG")){

        int size = bytes.size();
        text.sprintf("To save NO.%d JPG (%dbytes) done.", counter++, size);
        socket->writeDatagram((char *)(&size), sizeof(int), QHostAddress::Broadcast, PORT);
        socket->writeDatagram(bytes.data(), size, QHostAddress::Broadcast, 13925);
    }
    label->setText(text);
#else
    QString text;
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    if(pix.save(&buffer, "JPG")){
        int size = bytes.size();

        if(sizeof(int) != socket->writeDatagram((char *)(&size), sizeof(int), QHostAddress::Broadcast, PORT)){
            this->setWindowTitle("0:"+socket->errorString());
            return;
        }
        delay(SNDDELAY);

#define PACKMAX 1024
#if 0
        QString filename;
        filename.sprintf("%.2d.jpg", counter++);
        QFile f(filename);
        if(f.open(QIODevice::ReadWrite | QIODevice::Truncate)){
            char *p = bytes.data();
            while(1){
                if(size > PACKMAX){
                    if(PACKMAX != f.write(p, PACKMAX)){
                        qDebug() << "[TRACE]" << f.errorString() << endl;
                        qDebug("[TRACE]%s:%s:%d\n", __FILE__, __func__, __LINE__);
                        return;
                    }
                }else{
                    if(size != f.write(p, size)){
                        qDebug() << "[TRACE]" << f.errorString() << endl;
                        qDebug("[TRACE-%d]%s:%s:%d\n", size, __FILE__, __func__, __LINE__);
                        return;
                    }
                }

                size -= PACKMAX;
                p += PACKMAX;
            }
        }
        f.close();
#else
        char *p = bytes.data();
        text.sprintf("To snd JPG (%dbytes) done.", size);

        while(1){
            if(size > PACKMAX){
                if(PACKMAX != socket->writeDatagram(p, PACKMAX, QHostAddress::Broadcast, PORT)){
                    text = "1:"+socket->errorString();
                    this->setWindowTitle(text);
                    break;
                }
            }else{
                if(size != socket->writeDatagram(p, size, QHostAddress::Broadcast, PORT)){
                    text = "2:"+socket->errorString();
                    this->setWindowTitle(text);
                };
                break;
            }

            size -= PACKMAX;
            p += PACKMAX;

            delay(SNDDELAY);
        }
#endif
    }else{
        text = "QPixmap::save() fail";
        this->setWindowTitle(text);
    }
    label->setText(text);
#endif
}

void Udpsender::slotsavescreen()
{
    QDesktopWidget *desk = QApplication::desktop();
    QPixmap pix = QPixmap::grabWindow(desk->winId());

#if 0
    QString filename;
    filename.sprintf("%.2d.jpg", counter++);
    QFile f(filename);
    if(f.open(QIODevice::ReadWrite | QIODevice::Truncate)){
        if(pix.save(&f, "JPG")){
            qDebug() << "To save " << filename << " done." << endl;
            label->setText("To save "+filename+" done.");
        }else{
            qDebug() << "To save " << filename << " fail." << endl;
            label->setText("To save "+filename+" fail.");
        }
    }else{
        qDebug() << filename << "File open fail: " << f.errorString() << endl;
        label->setText("To open "+filename+" fail: "+f.errorString());
    }
#else
    this->sndpixmap(pix);
#endif
}

void Udpsender::slotstart()
{
    if(QString("&Start") == btn->text()){
        counter = 0;
        btn->setText(QString("&Stop"));
        tm->start(200);
    }else{
        btn->setText(QString("&Start"));
        tm->stop();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Udpsender snd;
    snd.show();

    return a.exec();
}

#include "main.moc"
