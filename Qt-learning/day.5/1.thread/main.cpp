/**/
#include <QApplication>
#include <QThread>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QString>
#include <QPushButton>
#include <QLabel>

#include <QComboBox>

#include <QDebug>

class MyThread : public QThread
{
    Q_OBJECT

public:
    explicit MyThread(QObject * parent = 0);
    ~MyThread();

public:
    void setLoopFlag(bool);

protected:
    void run() Q_DECL_OVERRIDE;

protected slots:
    void slotThrStartMsg();
    void slotThrDoneMsg();

signals:
    void sigcounter(int);

private:
    int counter;
    bool isLoop;
};

MyThread::MyThread(QObject *parent):
    QThread(parent)
{
    counter = 0;
    QThread::setTerminationEnabled(true);
}

MyThread::~MyThread()
{

}

void MyThread::setLoopFlag(bool flag)
{
    isLoop = flag;
    QThread::setTerminationEnabled(false);
}

void MyThread::run()
{
    counter = 0;
    while(isLoop){
        counter++;
        qDebug("Thread ID[%p] counter = %d\n", QThread::currentThreadId(), counter);
        emit sigcounter(counter);
        QThread::msleep(100);
    }
}

/*线程正常退出后，currentThreadId()得到的将是主线程的ID*/
void MyThread::slotThrStartMsg()
{
    qDebug("Thread ID[%p] Satrt\n", QThread::currentThreadId());
}

void MyThread::slotThrDoneMsg()
{
    qDebug("Thread ID[%p] Done\n", QThread::currentThreadId());
}

class MainWnd : public QWidget
{
    Q_OBJECT

public:
    explicit MainWnd(QWidget *parent = 0);
    ~MainWnd();

protected slots:
    void slotRun1();
    void slotRun2();

    void slotPrnlb1(int);
    void slotPrnlb2(int);

private:
    QGridLayout *gLayout;
    QHBoxLayout *hLayout1;
    QHBoxLayout *hLayout2;
    QPushButton *btntest1;
    QLabel *label1;
    QPushButton *btntest2;
    QLabel *label2;

    QComboBox *list;

    MyThread *thr1;
    MyThread *thr2;
};

MainWnd::MainWnd(QWidget *parent):
    QWidget(parent),
    thr1(new MyThread(parent)),
    thr2(new MyThread(parent))
{
    QObject::connect(thr1, SIGNAL(started()), thr1, SLOT(slotThrStartMsg()));
    QObject::connect(thr2, SIGNAL(started()), thr2, SLOT(slotThrStartMsg()));
    QObject::connect(thr1, SIGNAL(finished()), thr1, SLOT(slotThrDoneMsg()));
    QObject::connect(thr2, SIGNAL(finished()), thr2, SLOT(slotThrDoneMsg()));
    QObject::connect(thr1, SIGNAL(sigcounter(int)),\
                     this, SLOT(slotPrnlb1(int)), Qt::BlockingQueuedConnection);
    QObject::connect(thr2, SIGNAL(sigcounter(int)),\
                     this, SLOT(slotPrnlb2(int)), Qt::BlockingQueuedConnection);

    gLayout = new QGridLayout;
    hLayout1 = new QHBoxLayout;
    hLayout2 = new QHBoxLayout;

    btntest1 = new QPushButton(QString("&Start"), this);
    QObject::connect(btntest1, SIGNAL(clicked()),this,SLOT(slotRun1()));
    btntest2 = new QPushButton(QString("&Start"), this);
    QObject::connect(btntest2, SIGNAL(clicked()),this,SLOT(slotRun2()));
    label1 = new QLabel(QString("test"), this);
    label2 = new QLabel(QString("test"), this);

/*
QThread::IdlePriority	0	scheduled only when no other threads are running.
QThread::LowestPriority	1	scheduled less often than LowPriority.
QThread::LowPriority	2	scheduled less often than NormalPriority.
QThread::NormalPriority	3	the default priority of the operating system.
QThread::HighPriority	4	scheduled more often than NormalPriority.
QThread::HighestPriority	5	scheduled more often than HighPriority.
QThread::TimeCriticalPriority	6	scheduled as often as possible.
QThread::InheritPriority	7	use the same priority as the creating thread. This is the default.
*/
    list = new QComboBox(this);
    list->addItem("IdlePriority");
    list->addItem("LowestPriority");
    list->addItem("LowPriority");
    list->addItem("NormalPriority");
    list->addItem("HighPriority");
    list->addItem("HighestPriority");
    list->addItem("TimeCriticalPriority");
    list->addItem("InheritPriority");
    list->setCurrentIndex(7);
    list->setEditable(false);


    hLayout1->addWidget(label1);
    hLayout1->addStretch();
    hLayout1->addWidget(list);
    hLayout1->addWidget(btntest1);

    hLayout2->addWidget(label2);
    hLayout2->addStretch();
    hLayout2->addWidget(btntest2);

    gLayout->addLayout(hLayout1, 0, 0);
    gLayout->addLayout(hLayout2, 1, 0);

    this->setLayout(gLayout);

    qDebug("[MAIN]Thread ID[%p] Satrt\n", QThread::currentThreadId());
}

MainWnd::~MainWnd()
{
}

void MainWnd::slotRun1()
{
    /*
    QThread::IdlePriority	0	scheduled only when no other threads are running.
    QThread::LowestPriority	1	scheduled less often than LowPriority.
    QThread::LowPriority	2	scheduled less often than NormalPriority.
    QThread::NormalPriority	3	the default priority of the operating system.
    QThread::HighPriority	4	scheduled more often than NormalPriority.
    QThread::HighestPriority	5	scheduled more often than HighPriority.
    QThread::TimeCriticalPriority	6	scheduled as often as possible.
    QThread::InheritPriority	7	use the same priority as the creating thread. This is the default.
    */
    if(btntest1->text() == QString("&Start")){
        btntest1->setText(QString("&Stop"));
        QThread::Priority priority = (QThread::Priority)list->currentIndex();
        qDebug("id=%d, priority=%d\n", list->currentIndex(), priority);
        thr1->setLoopFlag(true);
        thr1->start(priority);
    }else{
        btntest1->setText(QString("&Start"));
        //thr1->terminate();
        thr1->setLoopFlag(false);
    }
}

void MainWnd::slotRun2()
{
    if(btntest2->text() == QString("&Start")){
        btntest2->setText(QString("&Stop"));
        thr2->setLoopFlag(true);
        thr2->start(QThread::TimeCriticalPriority);
    }else{
        btntest2->setText(QString("&Start"));
        thr2->terminate();
        //thr2->setLoopFlag(false);
    }
}

void MainWnd::slotPrnlb1(int counter)
{
    QString text;
    text.sprintf("COUNT: %d", counter);
    label1->setText(text);
}

void MainWnd::slotPrnlb2(int counter)
{
    QString text;
    text.sprintf("COUNT: %d", counter);
    label2->setText(text);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWnd w;
    w.show();

    return a.exec();
}

#include "main.moc"
