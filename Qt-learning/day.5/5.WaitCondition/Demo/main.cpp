#include <QCoreApplication>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>

#include <QDebug>

//#define PRODUCERFASTER

#define PROCDELAY 1
#define QUITDELAY 60000

#define MAXNR 24

static int array[MAXNR]= {0};

static QMutex mutex;
static QWaitCondition producer;
static QWaitCondition customer;

static int counter = 0;
static int k = 0;

class MyProducer : public QThread
{
    Q_OBJECT
public:
    explicit MyProducer(QObject * parent = 0):
        QThread(parent)
    {
        isRun = true;
        i = 0;
    }

    ~MyProducer()
    {
        qDebug("------MyProducer done------");
    }

public slots:
    void slotclose()
    {
        isRun = false;
        i = 0;
    }

protected:
    void run() Q_DECL_OVERRIDE
    {
        while(isRun){
            mutex.lock();
            if(counter >= MAXNR)
                producer.wait(&mutex);
            mutex.unlock();

            array[i] = k++;
            qDebug("[MyProducer]id=%d, value=%d", i, array[i]);
            i++;
            i %= MAXNR;

            mutex.lock();
            ++counter;
            mutex.unlock();

            customer.wakeOne();
#ifdef PRODUCERFASTER
            QThread::msleep(PROCDELAY);
#else
            QThread::msleep(1000*PROCDELAY);
#endif
       }
    }

private:
    int i;
    bool isRun;
};

class MyCustomer : public QThread
{
    Q_OBJECT
public:
    explicit MyCustomer(QObject * parent = 0):
        QThread(parent)
    {
        isRun = true;
        i = 0;
    }

    ~MyCustomer()
    {
        qDebug("------MyCustomer done------");
    }

public slots:
    void slotclose()
    {
        isRun = false;
        i = 0;
    }

protected:
    void run() Q_DECL_OVERRIDE
    {
        while(isRun){
            mutex.lock();
            if(counter <= 0){
                customer.wait(&mutex);
            }
            mutex.unlock();

            qDebug("[MyCustomer]id=%d, value=%d", i, array[i]);
            i++;
            i %= MAXNR;

            mutex.lock();
            counter--;
            mutex.unlock();

            producer.wakeOne();

#ifdef PRODUCERFASTER
            QThread::msleep(1000*PROCDELAY);
#else
            //QThread::msleep(PROCDELAY);
#endif
        }
    }

private:
    bool isRun;
    int i;
};

class MyCoreApplication : public QCoreApplication
{
    Q_OBJECT
public:
    explicit MyCoreApplication(MyProducer *p, MyCustomer *c, int argc=0, char ** argv=NULL):
        QCoreApplication(argc, argv),
        prod(p),
        cust(c)
    {
    }

    ~MyCoreApplication()
    {
    };

protected slots:
    void slotclose()
    {
        ::producer.wakeOne();
        ::customer.wakeOne();

        prod->slotclose();
        cust->slotclose();

        prod->wait();
        cust->wait();

        this->quit();
    }

private:
   MyProducer *prod;
   MyCustomer *cust;
};

int main(int argc, char *argv[])
{
    for(int i = 0; i < MAXNR; i++)
        array[i]= -1;

    MyProducer Producer;
    MyCustomer Customer;

    MyCoreApplication app(&Producer, &Customer);

    Producer.start();
    Customer.start();

    QTimer::singleShot(QUITDELAY, &app, SLOT(slotclose()));

    return app.exec();
}

#include "main.moc"
