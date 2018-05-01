#include "items.h"
#include <QPixmap>
#include <QGraphicsItem>
#include <QPainter>

PixItem::PixItem(QPixmap *pixmap, QGraphicsItem * parent):
    QGraphicsItem(parent)
{   
    pix = *pixmap;
}

QRectF PixItem::boundingRect() const
{
    return QRectF(-pix.width()/2, -pix.height()/2, pix.width(), pix.height());
}

void PixItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(-pix.width()/2,-pix.height()/2,pix);
}
