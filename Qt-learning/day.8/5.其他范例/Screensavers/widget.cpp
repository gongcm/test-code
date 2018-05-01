#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget * parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->connect(ui->pushButton, SIGNAL(clicked()),\
                  this, SLOT(slotTest()));

    tm = new QTimer(this);
    this->connect(tm, SIGNAL(timeout()), this, SLOT(slotBlackScreen()));

    bIsRun = false;
}

Widget::~Widget()
{
    tm->deleteLater();
    delete ui;
}

void Widget::slotTest()
{
    /*按钮模拟条件满足，开始黑屏*/
    bIsRun = true;
    /*主窗口隐藏*/
    this->hide();
    blkwnd = new BlackScreen(NULL,Qt::FramelessWindowHint);
    blkwnd->show();

    /*三秒后恢复*/
    tm->start(3000);
}

void Widget::slotBlackScreen()
{
    if(bIsRun){
        bIsRun = false;
        /*模拟条件满足，结束黑屏*/
        this->show();
        tm->stop();
        if(blkwnd->close()){
            qDebug("close wnd done.\n");
            /*回收资源*/
            blkwnd->deleteLater();
        }else{
            qDebug("close wnd fail.\n");
        }
    }
}

#include <QDesktopWidget>
BlackScreen::BlackScreen(QWidget * parent, Qt::WindowFlags f) :
    QWidget(parent,f)
{
    /*得到桌面大小，并设置窗口为桌面大小*/
    QDesktopWidget *desk = QApplication::desktop();
    QRect rect = desk->screenGeometry(-1);
    this->resize(rect.width(), rect.height());

    /*隐藏光标*/
    QApplication::setOverrideCursor(Qt::BlankCursor);
    this->setCursor(Qt::BlankCursor);
#if 0
    /*将窗口设置为：顶层窗口；无标题栏；工具条模式；背景透明；无输入焦点*/
    setWindowFlags( Qt::FramelessWindowHint |
                    Qt::Tool |
                    Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setFocusPolicy(Qt::NoFocus);
#endif
}

BlackScreen::~BlackScreen()
{
    /*显示光标，最好先保存光标样式，这里指定为箭头*/
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    this->setCursor(Qt::ArrowCursor);
}

#include <QPainter>
void BlackScreen::paintEvent(QPaintEvent *e)
{
    /*填充整个窗口，这样可以指定颜色或是图片*/
    //QPainter pt(this);
    //pt.fillRect(0,0,this->width(),this->height(), Qt::SolidPattern);
    return QWidget::paintEvent(e);
}
