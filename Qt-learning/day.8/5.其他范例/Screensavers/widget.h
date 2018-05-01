#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>

namespace Ui {
    class Widget;
}

class BlackScreen;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void slotTest();
    void slotBlackScreen();

private:
    Ui::Widget *ui;

    bool bIsRun;
    QTimer *tm;
    BlackScreen *blkwnd;
};

/*如果再实现进程的控制，黑屏期间休眠就完美了*/
/*实现黑屏的窗口，如果需要鼠标键盘激活窗口，则添加相应事件处理*/
class BlackScreen : public QWidget
{
    Q_OBJECT

public:
    explicit BlackScreen(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~BlackScreen();

protected:
    void paintEvent(QPaintEvent *e);
};


#endif // WIDGET_H
