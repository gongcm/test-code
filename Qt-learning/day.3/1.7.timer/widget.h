#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void slotbtnexit();
    void slotbtnonoff();
    void slotbtnstartstop();
    void slottimeout();

//private:
    //void timerEvent(QTimerEvent * event);

private:
    QTimer *timer;
    int tmid;
    Ui::Widget *ui;
};

#endif // WIDGET_H
