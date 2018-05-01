#ifndef PAINTAREA_H
#define PAINTAREA_H

#include <QWidget>

class PaintArea : public QWidget
{
    Q_OBJECT
public:

    PaintArea(QWidget *parent=0);     
   
    void setScale(int);
    void setTranslate(int);
    void setRotate(int);
    void setShear(int);

    void paintEvent(QPaintEvent *);
  
private:
    qreal scale;
    int translate;
    qreal angle;
    qreal shear;
};

#endif 
