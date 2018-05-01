#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QEvent>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
     void paintEvent(QPaintEvent * event);
     void timerEvent(QTimerEvent * event);

     void mouseMoveEvent(QMouseEvent * event);

private:
    Ui::Widget *ui;
    QPixmap image;
    int *arr;
    int count;
};

#endif // WIDGET_H
