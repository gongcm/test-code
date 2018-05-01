#include "widget.h"
#include "ui_widget.h"
#include <QBuffer>
#include <QDebug>

#define MEMSIZE 1024

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    sharemem = new QSharedMemory(QString("demo"),this);

    QObject::connect(this, SIGNAL(destroyed()), this, SLOT(close()));

    if(!sharemem->create(MEMSIZE,QSharedMemory::ReadWrite)){
        if(QSharedMemory::AlreadyExists != sharemem->error()){
            qDebug() << "ERR: " << sharemem->errorString() << endl;
            ui->label->setText("ERR:"+sharemem->errorString());
        }
    }

    if(!sharemem->attach(QSharedMemory::ReadWrite)){
        ui->label->setText("1-ERR:"+sharemem->errorString());
    }

    QObject::connect(ui->rdbtn, SIGNAL(clicked()), this, SLOT(slotrdmem()));
    QObject::connect(ui->wrbtn, SIGNAL(clicked()), this, SLOT(slotwrmem()));
    QObject::connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(slotwrmem()));
}

Widget::~Widget()
{
    sharemem->detach();
    delete ui;
}

void Widget::slotrdmem()
{
    qDebug("%s:%d", __func__, __LINE__);
    QString text;

    sharemem->lock();
    if(sharemem->isAttached()){
        text.sprintf("CONTEXT:%s", (char*)sharemem->constData()); 
		ui->label->setText(text);
    }else{
        ui->label->setText("Read fail:"+sharemem->errorString());
    }
	
	sharemem->unlock();  
}

void Widget::slotwrmem()
{
    qDebug("%s:%d", __func__, __LINE__);
    std::string str = ui->lineEdit->text().toStdString();

    sharemem->lock();
    if(sharemem->isAttached())
        memcpy(sharemem->data(), str.c_str(), str.length());
    else
        ui->label->setText("write fail:"+sharemem->errorString());
    sharemem->unlock();
}
