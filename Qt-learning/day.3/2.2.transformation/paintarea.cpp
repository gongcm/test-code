#include "paintarea.h"
#include <math.h>
#include <QPainter>
#include <QDebug>

#define PI 3.1415926535

PaintArea::PaintArea(QWidget *parent):
    QWidget(parent)
{
    setPalette(QPalette(Qt::white));
    setAutoFillBackground(true);
    
    scale = 1;
    translate = 0;
    angle = 0;
    shear = 0;
}

void PaintArea::setScale(int s)
{
    qDebug() << "scale: " << s << endl;
    scale = ((qreal)s)/50.0;
    update();
}

void PaintArea::setRotate(int r)
{
    angle = ((qreal)r)/2.0;
    update();
}

void PaintArea::setTranslate(int t)
{
    translate = t-99;
    update();
}

void PaintArea::setShear(int s)
{
    shear = ((qreal)s)/10.0;
    update();
}

void PaintArea::paintEvent(QPaintEvent *)
{ 
    QPainter p(this);   
    
    QPainterPath path;

    int w = this->width()/6;
    int h = this->height()/6;

#define OFFSET 3
    path.addRect(OFFSET,OFFSET,w-OFFSET,h-OFFSET);
            
    p.translate(3*w,3*h);	//  把窗体的坐标原点移到中心点
    p.rotate(angle);
    p.scale(scale,scale);
    p.translate(translate,translate);
    p.shear(shear, shear);

    p.setPen(Qt::red);
    p.drawLine(0, 0, w, 0);
    p.drawLine(w-OFFSET, -OFFSET, w, 0);
    p.drawLine(w-OFFSET, OFFSET, w, 0);
    p.drawText(w, OFFSET, tr("x"));

    p.drawLine(0, 0, 0, h);
    p.drawLine(-OFFSET, h-OFFSET, 0, h);
    p.drawLine(OFFSET, h-OFFSET, 0, h);
    p.drawText(OFFSET, h, tr("y"));
    p.drawText(-10,-6,"(0,0)");   
    
    p.setPen(Qt::blue);
    p.drawPath(path); 
}
