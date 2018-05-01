#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>

#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setMinimumWidth(240);

    QObject::connect(ui->btntest, SIGNAL(clicked()), this, SLOT(slotclick()));
    QObject::connect(ui->btnexit, SIGNAL(clicked()), this, SLOT(close()));
}

Widget::~Widget()
{
    delete ui;
}

/*获得当前实例的方法qApp，QApplication::instance()*/
void Widget::slotclick()
{
    static int i = 0;
    if(10 == i){
        i = 0;
    }

    QString text;
    text.sprintf("notify key event");
    QKeyEvent keypress = QKeyEvent(QEvent::KeyPress, Qt::Key_A+i, Qt::NoModifier, text);
    QApplication::instance()->notify(ui->textEdit, &keypress);
    text.sprintf("[%d]", i);
    keypress = QKeyEvent(QEvent::KeyPress, Qt::Key_1+i, Qt::NoModifier, text);
    QApplication::sendEvent(ui->textEdit, &keypress);
    i++;

    static QKeyEvent keyreturn = QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, NULL);
    qApp->postEvent(ui->textEdit, &keyreturn, Qt::HighEventPriority);
}
