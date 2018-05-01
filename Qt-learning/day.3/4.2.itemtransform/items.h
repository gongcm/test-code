#ifndef ITEMS_H
#define ITEMS_H

#include <QGraphicsItem>
#include <QPixmap>

class QGraphicsSceneMouseEvent;

class PixItem : public QGraphicsItem
{
public:
    PixItem(QPixmap *pixmap, QGraphicsItem * parent = 0);

private:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPixmap pix;
};


#endif 	// ITEMS_H
