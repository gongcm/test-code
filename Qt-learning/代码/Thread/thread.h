#ifndef THREAD_H
#define THREAD_H
#include <QThread>

class Thread : public QThread
{
      Q_OBJECT

public:
    Thread();

signals:
    void updateText(QString text);

protected:
    void run();


};

#endif // THREAD_H
