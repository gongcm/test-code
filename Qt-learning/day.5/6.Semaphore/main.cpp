#include <QCoreApplication>
#include <QThread>
#include <QSemaphore>
#include <QTimer>

#include <QDebug>

//#define PRODUCERFASTER

#define PROCDELAY 10
#define QUITDELAY 10000

static int value = 0;

static QSemaphore producer(1);
static QSemaphore customer(0);

class MyProducer : public QThread
{
    Q_OBJECT
public:
    explicit MyProducer(QObject * parent = 0):
        QThread(parent)
    {
        qDebug("------MyProducer start------");
        this->setTerminationEnabled(true);
    }

    ~MyProducer()
    {
        qDebug("------MyProducer done------");
    }

protected:
    void run() Q_DECL_OVERRIDE
    {
        while(1){
            producer.acquire();
            value++;
            qDebug("[MyProducer]value=%d", value);
            customer.release();

#ifdef PRODUCERFASTER
            QThread::msleep(PROCDELAY);
#else
            QThread::msleep(100*PROCDELAY);
#endif
       }
    }
};

class MyCustomer : public QThread
{
    Q_OBJECT
public:
    explicit MyCustomer(QObject * parent = 0):
        QThread(parent)
    {
        qDebug("------MyCustomer start------");
        this->setTerminationEnabled(true);
    }

    ~MyCustomer()
    {
        qDebug("------MyCustomer done------");
    }

protected:
    void run() Q_DECL_OVERRIDE
    {
        while(1){
            customer.acquire();
            qDebug("[MyCustomer]value=%d", value);
            producer.release();

#ifdef PRODUCERFASTER
            QThread::msleep(100*PROCDELAY);
#else
            QThread::msleep(PROCDELAY);
#endif
        }
    }
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
        prod->terminate();
        cust->terminate();

        prod->wait();
        cust->wait();

        quit();
    }

private:
   MyProducer *prod;
   MyCustomer *cust;
};

int main(int argc, char *argv[])
{
    MyProducer Producer;
    MyCustomer Customer;

    MyCoreApplication app(&Producer, &Customer, argc, argv);


    Customer.start();
    QThread::sleep(10);

    Producer.start();

    QTimer::singleShot(QUITDELAY, &app, SLOT(slotclose()));

    return app.exec();
}

#include "main.moc"
