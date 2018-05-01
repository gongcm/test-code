#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>

#include <QDebug>

#define PROCDELAY 1000

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject * parent = 0):
        QThread(parent)
    {
        qDebug("------MyThread start------");

        isRun = true;
        isSuspend = false;
        setTerminationEnabled(true);

        counter = 0;

        mutex = new QMutex();
        cond = new QWaitCondition;
    }

    ~MyThread()
    {
        delete mutex;
        delete cond;
        qDebug("------MyThread done------");
    }

public slots:
    void slotExit()
    {
        mutex->lock();
        isRun = false;
        mutex->unlock();
    }

    void slotSuspend()
    {
        mutex->lock();
        isSuspend = true;
        mutex->unlock();
    }

    void slotWakeup()
    {
        mutex->lock();
        if(isSuspend){
            isSuspend = false;
            mutex->unlock();
            cond->wakeOne();
        }else{
            mutex->unlock();
        }
    }

signals:
    void sigExit();
    void sigSuspend();
    void sigWakeup();
    void sigTerminate();
    void sigRunMsg(QString);

protected:
    void run() Q_DECL_OVERRIDE
    {
        while(1){
            mutex->lock();
            if(!isRun){
                mutex->unlock();
                break;
            }

            if(isSuspend){
                cond->wait(mutex);
            }
            mutex->unlock();

            QString text;
            text.sprintf("[ID=%.8p] counter = %.8d", QThread::currentThreadId(), counter++);
            emit sigRunMsg(text);
            QThread::msleep(PROCDELAY);
       }
    }

private:
    int counter;
    bool isRun;
    bool isSuspend;
    QMutex *mutex;
    QWaitCondition *cond;
};

class MyWnd : public QWidget
{
    Q_OBJECT
public:
    explicit MyWnd(QWidget * parent = 0):
        QWidget(parent)
    {
        qDebug("------MyWnd start------");

        gLayout = new QGridLayout;
        hLayout1 = new QHBoxLayout;
        hLayout2 = new QHBoxLayout;

        btntest1 = new QPushButton(QString("&Start"), this);
        QObject::connect(btntest1, SIGNAL(clicked()),this,SLOT(slotRun1()));
        btntest2 = new QPushButton(QString("&Start"), this);
        QObject::connect(btntest2, SIGNAL(clicked()),this,SLOT(slotRun2()));
        label1 = new QLabel(QString("......"), this);
        label2 = new QLabel(QString("......"), this);

        hLayout1->addWidget(label1);
        hLayout1->addStretch();
        hLayout1->addWidget(btntest1);

        hLayout2->addWidget(label2);
        hLayout2->addStretch();
        hLayout2->addWidget(btntest2);

        gLayout->addLayout(hLayout1, 0, 0);
        gLayout->addLayout(hLayout2, 1, 0);

        this->setLayout(gLayout);


        thr1 = new MyThread(this);
        thr2 = new MyThread(this);
        QObject::connect(thr1, SIGNAL(sigRunMsg(QString)),\
                         this, SLOT(slotPrnlb1(QString)));
        QObject::connect(thr2, SIGNAL(sigRunMsg(QString)),\
                         this, SLOT(slotPrnlb2(QString)));

        thr1->start();
        thr1->slotSuspend();
        thr2->start();
        thr2->slotSuspend();
    }

    ~MyWnd()
    {
        thr1->slotWakeup();
        thr2->slotWakeup();

        thr1->slotExit();
        thr2->slotExit();

        thr1->wait();
        thr2->wait();

        thr1->deleteLater();
        thr2->deleteLater();

        qDebug("------MyWnd done------");
    }

protected slots:
    void slotRun1()
    {
        if(btntest1->text() == QString("&Start")){
            btntest1->setText(QString("&Stop"));
            thr1->slotWakeup();
        }else{
            btntest1->setText(QString("&Start"));
            thr1->slotSuspend();
        }
    }

    void slotRun2()
    {
        if(btntest2->text() == QString("&Start")){
            btntest2->setText(QString("&Stop"));
            thr2->slotWakeup();
        }else{
            btntest2->setText(QString("&Start"));
            thr2->slotSuspend();
        }
    }

    void slotPrnlb1(QString text)
    {
        label1->setText(text);
    }

    void slotPrnlb2(QString text)
    {
        label2->setText(text);
    }

private:
    MyThread *thr1;
    MyThread *thr2;

    QGridLayout *gLayout;
    QHBoxLayout *hLayout1;
    QHBoxLayout *hLayout2;
    QPushButton *btntest1;
    QPushButton *btntest2;
    QLabel *label1;
    QLabel *label2;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MyWnd wnd;
    wnd.show();

    return app.exec();
}

#include "main.moc"
