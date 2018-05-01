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

#define PORT 9999

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
    }else{
        text = "QPixmap::save() fail";
        this->setWindowTitle(text);
    }
    label->setText(text);
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
        tm->start(1000);
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
