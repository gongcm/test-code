#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->CBextend->setEnabled(true);
    QObject::connect(ui->CBextend, SIGNAL(clicked(bool)),
                     this, SLOT(slotreduce(bool)));

//    QObject::connect(ui->btnok, SIGNAL(clicked()),
//                     this, SLOT(close()));

    ui->checkBox->hide();
    ui->checkBox_2->hide();

    this->setWindowTitle("Demo");

    this->setEnabled(true);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slotreduce(bool flag)
{
    if(flag){
        ui->checkBox->show();
        ui->checkBox_2->show();
    }else{
        ui->checkBox->hide();
        ui->checkBox_2->hide();
    }
}
