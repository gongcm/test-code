#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    qDebug() << QDir::currentPath();

    ui->inputButton->setEnabled(false);
    ui->showButton->setEnabled(false);

    QFile file ("/Users/cyg/test.txt");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate)){
        qDebug() << "Fail to open file : " << file.errorString();
        return;
    }

    QTextStream ds(&file);
    ds <<123 << showbase << hex <<0x123;

    file.flush();

    ds.seek(0);

    QString str;

    ds >>hex>>str;

    qDebug() << "str :" << str << endl;

    file.close();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_inputButton_clicked()
{
    QFile file ("/Users/cyg/test.txt");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)){
        qDebug() << "Fail to open file : " << file.errorString();
        return;
    }

    QTextStream ts(&file);
    ts << ui->inputText->toPlainText();
    ui->inputText->clear();

    file.close();

    return;
}

void Widget::on_showButton_clicked()
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Fail to open file : " << file.errorString();
        return;
    }

    QTextStream ts(&file);
    QString string = ts.readAll();
    ui->showText->setText(string);

    file.close();

    return;
}

void Widget::on_fileBrowse_clicked()
{
    filePath = QFileDialog::getOpenFileName\
            (this,tr("Open file"),".",tr("Text files (*.txt);;C files (*.c)"));

    qDebug() << "filePath" << filePath;

    if(filePath.isEmpty()){
        QMessageBox::information(this,tr("Path"),tr("You don't select any files!"));
    }else{
        QMessageBox::information(this,tr("Path"),tr("You selected ") + filePath);
    }

    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Fail to open file : " << file.errorString();
        return;
    }

    QTextStream ts(&file);
    QString string = ts.readAll();
    ui->inputText->setText(string);

    file.close();

    ui->inputButton->setEnabled(true);
    ui->showButton->setEnabled(true);
}

