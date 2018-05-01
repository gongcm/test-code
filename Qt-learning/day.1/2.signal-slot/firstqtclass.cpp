#include "firstqtclass.h"
#include "ui_firstqtclass.h"
#include <QtDebug>

FirstQtClass::FirstQtClass(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FirstQtClass)
{
    ui->setupUi(this);
    btn = new QPushButton(this);

    this->connect(ui->btn, SIGNAL(clicked()),
                  this, SLOT(slotBtn()));
    this->connect(this, SIGNAL(sigDemo(QString,int)),
                  this, SLOT(slotDemo(QString)));
}

FirstQtClass::~FirstQtClass()
{
    delete ui;
}

void FirstQtClass::slotDemo(QString str)
{
    qDebug() << str.toStdString().data();
    this->disconnect(this, SIGNAL(sigDemo(QString,int)),
                  this, SLOT(slotDemo(QString)));
}

void FirstQtClass::slotBtn()
{
    emit sigDemo(QString("hello"), 123);
}
