#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QDesktopWidget>
#include <QDebug>
#include <QLabel>

#include <QToolTip>

#define W 800
#define H 320

#define STEP 10

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setMaximumSize(W,H);
    this->setMinimumSize(W,H);

    arr = new int[W*H];

    for(int i = 0; i < W*H; i++)
        arr[i]=0;

    count = 0;

    image = QPixmap(W,H);
    image.fill(QColor(255,255,255));

    setMouseTracking(true);

    this->startTimer(1000);
}

Widget::~Widget()
{
    delete ui;
    delete arr;
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawPixmap(0, 0, W, H, image);
}

void Widget::timerEvent(QTimerEvent *event)
{
    QPainter p(&image);

    QPen pen = QPen(QColor(0,0,255));
    pen.setWidth(2);
    p.setPen(pen);

    image.fill(QColor(255,255,255));

    arr[count++] = qrand()% (W/2);
    update();

    if(count == W/STEP){
        for(int i = 1; i < W; i++){
            arr[i-1] = arr[i];
        }
        count--;
    }

    int id = 0;
    for(int i = 0; i < W-STEP; i+=STEP){

        p.drawLine(i, arr[id]+W/8, i+STEP, arr[id+1]+W/8);
        id++;
    }
}

#include <QMouseEvent>

void Widget::mouseMoveEvent(QMouseEvent * event)
{
    static int i = 0;
    int x = event->globalX();
    int y = event->globalY();
    QToolTip::showText(QPoint(x,y),"hello");


    QWidget::mouseMoveEvent(event);
}
