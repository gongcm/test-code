#include "thread.h"
#include <QDebug>
#include <QTimer>

Thread::Thread()
{
     QTimer *timer = new QTimer(this);
}

void Thread::run()
{
  int count = 0;



  while(1){
      qDebug() << "hello word";

      QString text = QString("Hello word%1\n").arg(count ++);
      emit updateText(text);
      QThread::sleep(1);
  }

}
