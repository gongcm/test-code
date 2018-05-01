#include "widget.h"
#include "ui_widget.h"
#include "thread.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    Thread *thread = new Thread;
    connect(thread,SIGNAL(updateText(QString)),this,SLOT(updateTextEdit(QString)));

    thread->start();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::updateTextEdit(QString text)
{
    ui->textEdit->insertPlainText(text);
}
