#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QThread>

class MyWnd : public QWidget
{
    Q_OBJECT

public:
    explicit MyWnd(QWidget *parent = 0);
    ~MyWnd();

private slots:
    void slotclick();
    void slotexit();

private:
    QPushButton *btnok;
    QPushButton *btncancel;
    QHBoxLayout *hlayout;
    bool bIsRun;
};

MyWnd::MyWnd(QWidget *parent) :
    QWidget(parent)
{
    btnok = new QPushButton(QString("&Test"), this);
    btncancel = new QPushButton(QString("E&xit"), this);

    QObject::connect(btnok, SIGNAL(clicked()), this, SLOT(slotclick()));
    QObject::connect(btncancel, SIGNAL(clicked()), this, SLOT(slotexit()));
    QObject::connect(btncancel, SIGNAL(clicked()), this, SLOT(close()));

    hlayout = new QHBoxLayout;
    hlayout->addWidget(btnok);
    hlayout->addStretch();
    hlayout->addWidget(btncancel);

    this->setLayout(hlayout);
    this->setMinimumWidth(240);

    bIsRun = true;
}

MyWnd::~MyWnd()
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;
}

void MyWnd::slotclick()
{
    unsigned int i = 0;
    QString text;
    while(bIsRun){
        text.sprintf("TEST: %d", i++);
        qDebug() << text << endl;
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
    }
}

void MyWnd::slotexit()
{
    bIsRun = false;
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyWnd w;
    w.show();

    return a.exec();
}

#include "main.moc"
