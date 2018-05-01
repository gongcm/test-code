/**/
#include <QCoreApplication>
#include <QThread>

#include <QMutex>
#include <QMutexLocker>

#include <QDebug>

#define ARRMAX 12
static int arr[ARRMAX];

#define USELOCK
#define USEMUTEX

static QMutex mutex;

static void initarr(int value)
{
#ifdef USEMUTEX
#ifdef USELOCK
    mutex.lock();
#endif

    for(int i = 0; i < ARRMAX; i++){
        arr[i] = value;
        QThread::msleep(1);
    }

    for(int i = 0; i < ARRMAX; i++){
        qDebug("value=%d, ID=%p, arr[%d]=%d", value, QThread::currentThreadId(), i, arr[i]);
    }
#ifdef USELOCK
    mutex.unlock();
#endif

#else

#ifdef USELOCK
    QMutexLocker lock(&mutex);
#endif
    for(int i = 0; i < ARRMAX; i++){
        arr[i] = value;
        QThread::msleep(1);
    }

    for(int i = 0; i < ARRMAX; i++){
        qDebug("value=%d, ID=%p, arr[%d]=%d", value, QThread::currentThreadId(), i, arr[i]);
    }
#endif
}

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject * parent = 0);
    ~MyThread();

public:
    void setValue(int);

protected:
    void run() Q_DECL_OVERRIDE;

private:
    int value;
};

MyThread::MyThread(QObject *parent):
    QThread(parent)
{
    value = 0;
}

MyThread::~MyThread()
{
}

void MyThread::run()
{
    initarr(value);
}

void MyThread::setValue(int i)
{
    value = i;
}

int main(int argc, char *argv[])
{
    MyThread thr;
    thr.setValue(1);
    thr.start();

    initarr(3);

    thr.wait();
}

#include "main.moc"
