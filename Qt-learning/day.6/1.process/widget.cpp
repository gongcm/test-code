#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QDir>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->label->setText(QString("---"));
    ui->label->setAlignment(Qt::AlignCenter);
    this->setMaximumSize(320,240);
    this->setMinimumSize(320,240);

    process = new QProcess(this);
    QObject::connect(process, SIGNAL(started()), this, SLOT(slotstarted()));
    QObject::connect(process, SIGNAL(destroyed()), this, SLOT(slotdestroyed()));
    QObject::connect(process, SIGNAL(finished(int)), this, SLOT(slotfinished(int)));

    QObject::connect(ui->btnstart, SIGNAL(clicked()), this, SLOT(slotstart()));
    QObject::connect(ui->btnkill, SIGNAL(clicked()), this, SLOT(slotkill()));
    QObject::connect(ui->btnterminate, SIGNAL(clicked()), this, SLOT(slotterminate()));
}

Widget::~Widget()
{
    qDebug() << "~Widget" << endl;
    delete ui;
}

void Widget::slotstart()
{
#if 1
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QString path = ";"+QDir::currentPath()+"\\debug";
    qDebug() << path << endl;

    env.insert("PATH", env.value("Path") + path);
    process->setProcessEnvironment(env);
    process->start("process");
#else
    //process->start("process");
    QStringList arg;
    arg << "1.txt";
    QProcess::execute("notepad", arg);
    //QProcess::startDetached("notepad");
#endif
}

void Widget::slotkill()
{
    process->kill();
}

void Widget::slotterminate()
{
    process->terminate();
}

void Widget::slotstarted()
{
    qDebug() << "slotstarted" << endl;
    ui->label->setText(QString("started"));
}

void Widget::slotdestroyed()
{
    qDebug() << "slotdestroyed" << endl;
    ui->label->setText(QString("destroyed"));
}

void Widget::slotfinished(int value)
{
    qDebug() << "slotfinished: " << value << endl;
    ui->label->setText(QString("finished"));
}
