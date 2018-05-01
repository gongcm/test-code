#include "widget.h"
#include "ui_widget.h"

#include <QLibrary>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(slotadd()));
    ui->label->setText("Demo for call DLL API");
}

Widget::~Widget()
{
    delete ui;
}

typedef int (*PFUNADD)(int, int);

void Widget::slotadd()
{
#if 0
    QLibrary lib("DLL.dll");
    if(!lib.load()){
        ui->label->setText(QString("Load DLL.dll fail."));
    }else{
        PFUNADD pfun = (PFUNADD)lib.resolve("add");
        if(pfun){
            QString str;
            int x = ui->lineEdit->text().toInt();
            int y = ui->lineEdit_2->text().toInt();
            str.sprintf("ADD RESULT: %d=%d+%d", pfun(x,y), x, y);
            ui->label->setText(str);
        }

        lib.unload();
    }
#else
    PFUNADD pfun = (PFUNADD) QLibrary::resolve("DLL.dll", "add");
    if(pfun){
        QString str;
        int x = ui->lineEdit->text().toInt();
        int y = ui->lineEdit_2->text().toInt();
        str.sprintf("ADD RESULT: %d=%d+%d", pfun(x,y), x, y);
        ui->label->setText(str);
    }
#endif
}
