#include "widget.h"
#include "ui_widget.h"

#include <QTimer>
#include <QTime>

#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QObject::connect(ui->btnquit, SIGNAL(clicked()), this, SLOT(slotbtnexit()));
    QObject::connect(ui->btnevent, SIGNAL(clicked()), this, SLOT(slotbtnonoff()));
    QObject::connect(ui->btntimer, SIGNAL(clicked()), this, SLOT(slotbtnstartstop()));

    timer = new QTimer(this);
    //QObject::connect(timer, SIGNAL(timeout()), this, SLOT(slottimeout()));

    tmid = -1;
}

Widget::~Widget()
{
    delete ui;
}

#if 0
void Widget::timerEvent(QTimerEvent * event)
{
    qDebug() << "timerId()=" << event->timerId() << "; tmid=" << tmid << "; timer=" << timer->timerId() << endl;
    QTime tm = QTime::currentTime();
    QString text;
    text.sprintf("%.2d:%.2d:%.2d", tm.hour(), tm.minute(), tm.second());
    ui->label->setText(text);
}
#endif

void Widget::slotbtnexit()
{
    QTimer::singleShot(3000, this, SLOT(close()));
}

void Widget::slotbtnonoff()
{
    if(ui->btnevent->text() == QString("&On")){
        ui->btnevent->setText("&Off");
        tmid = this->startTimer(1000);
    }else{
        ui->btnevent->setText("&On");
        this->killTimer(tmid);
    }
}

void Widget::slotbtnstartstop()
{
    if(ui->btntimer->text() == QString("&Start")){
        ui->btntimer->setText("&Stop");
        timer->start(1000);
    }else{
        ui->btntimer->setText("&Start");
        timer->stop();
    }
}

void Widget::slottimeout()
{
    qDebug() << "tmid=" << tmid << "; timer=" << timer->timerId() << endl;
    QTime tm = QTime::currentTime();
    ui->lbtimer->setText(tm.toString(Qt::SystemLocaleLongDate));
}
