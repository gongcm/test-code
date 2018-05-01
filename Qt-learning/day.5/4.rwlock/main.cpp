/**/
#include <QCoreApplication>
#include <QThread>
#include <QReadWriteLock>
#include <QDebug>

#define ARRMAX 6
static int arr[ARRMAX];

#ifdef RWLOCK
static QReadWriteLock rwlock;
#endif

static void initarr(int value)
{
#ifdef RWLOCK
    rwlock.lockForWrite();
#endif
    for(int i = 0; i < ARRMAX; i++){
        arr[i] = value;
        qDebug("ID=%p, arr[%d]=%d",
               QThread::currentThreadId(), i, arr[i]);
        QThread::sleep(1);
    }
#ifdef RWLOCK
    rwlock.unlock();
#endif
}

static void prnarr()
{
#ifdef RWLOCK
    rwlock.lockForRead();
#endif
    for(int i = 0; i < ARRMAX; i++){
        qDebug("ID=%p, arr[%d]=%d",
               QThread::currentThreadId(), i, arr[i]);
        QThread::sleep(1);
    }
#ifdef RWLOCK
    rwlock.unlock();
#endif
}

class WThread : public QThread
{
    Q_OBJECT
public:
    explicit WThread(int val=0, QObject *parent=NULL);
protected:
    void run() Q_DECL_OVERRIDE;

private:
    int value;
};

WThread::WThread(int val,QObject *parent):
    QThread(parent)
{
    value = val;
}

void WThread::run()
{
    initarr(value);
}

class RThread : public QThread
{
    Q_OBJECT
public:
    explicit RThread(QObject *parent=NULL);

protected:
    void run() Q_DECL_OVERRIDE;
};

RThread::RThread(QObject *parent):
    QThread(parent)
{
}

void RThread::run()
{
    prnarr();
}

int main(int argc, char *argv[])
{
    WThread w(1);
    w.start();

    RThread r1;
    r1.start();

    RThread r2;
    r2.start();

    w.wait();
    r1.wait();
    r2.wait();
}

#include "main.moc"
