#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>

class MyWnd : public QWidget
{
    Q_OBJECT

public:
    explicit MyWnd(QWidget *parent = 0);
    ~MyWnd();

private:
    //特定事件处理器
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void moveEvent(QMoveEvent * event);

    //事件处理器
    bool event(QEvent * ev);

    QLabel *label;
    QHBoxLayout *hlayout;
};

MyWnd::MyWnd(QWidget *parent) :
    QWidget(parent)
{
    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setText(QString("Demo for event"));

    hlayout = new QHBoxLayout;
    hlayout->addWidget(label);

    this->setLayout(hlayout);
    this->setMinimumWidth(240);
}

MyWnd::~MyWnd()
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;

    label->deleteLater();
    delete hlayout;
}

void MyWnd::keyPressEvent(QKeyEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;

    QString text;
    text.sprintf("keyPressEvent: %d", event->key());
    qDebug() << text << endl;

    QWidget::keyPressEvent(event);
}

void MyWnd::keyReleaseEvent(QKeyEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;
    QString text;
    text.sprintf("keyReleaseEvent: %d", event->key());
    qDebug() << text << endl;
}

void MyWnd::mouseDoubleClickEvent(QMouseEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;

    QString text;
    switch(event->button()){
    case Qt::LeftButton:
        text.sprintf("LeftButton");
        break;
    case Qt::RightButton:
        text.sprintf("RightButton");
        break;
    case Qt::MidButton:
        text.sprintf("MidButton");
        break;
    default:
        text.sprintf("default");
    }

    qDebug() << "mouseDoubleClickEvent:" << text << endl;
}

void MyWnd::mouseMoveEvent(QMouseEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;
    QString text;
    text.sprintf("mouseMoveEvent: %d,%d; %d,%d", event->pos().x(), event->pos().y(),
                 event->globalPos().x(), event->globalPos().y());
    qDebug() << text << endl;
}

void MyWnd::mousePressEvent(QMouseEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;

    QString text;
    switch(event->button()){
    case Qt::LeftButton:
        text.sprintf("LeftButton");
        break;
    case Qt::RightButton:
        text.sprintf("RightButton");
        break;
    case Qt::MidButton:
        text.sprintf("MidButton");
        break;
    default:
        text.sprintf("default");
    }

    qDebug() << "mousePressEvent:" << text << endl;
}

void MyWnd::mouseReleaseEvent(QMouseEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;
    QString text;
    switch(event->button()){
    case Qt::LeftButton:
        text.sprintf("LeftButton");
        break;
    case Qt::RightButton:
        text.sprintf("RightButton");
        break;
    case Qt::MidButton:
        text.sprintf("MidButton");
        break;
    default:
        text.sprintf("default");
    }

    qDebug() << "mouseReleaseEvent:" << text << endl;
}

void MyWnd::moveEvent(QMoveEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;
    QString text;
    text.sprintf("moveEvent: %d,%d; %d,%d", event->pos().x(), event->pos().y(),
                 event->oldPos().x(), event->oldPos().y());
    qDebug() << text << endl;
}

#define EVENTDONE

bool MyWnd::event(QEvent * ev)
{
    QString text;

    switch (ev->type()) {
    case QEvent::KeyPress:
        text.sprintf("keyPressEvent: %d", ((QKeyEvent *)ev)->key());
        label->setText(text);
#ifdef EVENTDONE
        return true;
#else
        break;
#endif

    case QEvent::KeyRelease:
        text.sprintf("keyReleaseEvent: %d", ((QKeyEvent *)ev)->key());
        label->setText(text);
#ifdef EVENTDONE
        return true;
#else
        break;
#endif

    case QEvent::MouseButtonDblClick:
        switch(((QMouseEvent *)ev)->button()){
        case Qt::LeftButton:
            text.sprintf("LeftButton");
            break;
        case Qt::RightButton:
            text.sprintf("RightButton");
            break;
        case Qt::MidButton:
            text.sprintf("MidButton");
            break;
        default:
            text.sprintf("default");
        }

        label->setText("mouseDoubleClickEvent:"+text);
#ifdef EVENTDONE
        return true;
#else
        break;
#endif

    case QEvent::MouseMove:
        text.sprintf("mouseMoveEvent: %d,%d; %d,%d", ((QMouseEvent*)ev)->pos().x(), ((QMouseEvent*)ev)->pos().y(),
                     ((QMouseEvent*)ev)->globalPos().x(), ((QMouseEvent*)ev)->globalPos().y());
        label->setText(text);
#ifdef EVENTDONE
        return true;
#else
        break;
#endif

    case QEvent::MouseButtonPress:
        switch(((QMouseEvent *)ev)->button()){
        case Qt::LeftButton:
            text.sprintf("LeftButton");
            break;
        case Qt::RightButton:
            text.sprintf("RightButton");
            break;
        case Qt::MidButton:
            text.sprintf("MidButton");
            break;
        default:
            text.sprintf("default");
        }

        label->setText("mousePressEvent:"+text);
#ifdef EVENTDONE
        return true;
#else
        break;
#endif

    case QEvent::MouseButtonRelease:
        switch(((QMouseEvent *)ev)->button()){
        case Qt::LeftButton:
            text.sprintf("LeftButton");
            break;
        case Qt::RightButton:
            text.sprintf("RightButton");
            break;
        case Qt::MidButton:
            text.sprintf("MidButton");
            break;
        default:
            text.sprintf("default");
        }

        label->setText("mouseReleaseEvent:"+text);
#ifdef EVENTDONE
        return true;
#else
        break;
#endif

    case QEvent::Move:
        text.sprintf("moveEvent: %d,%d; %d,%d", ((QMoveEvent*)ev)->pos().x(), ((QMoveEvent*)ev)->pos().y(),
                     ((QMoveEvent*)ev)->oldPos().x(), ((QMoveEvent*)ev)->oldPos().y());
        label->setText(text);
#ifdef EVENTDONE
        return true;
#else
        break;
#endif
    };

    return QWidget::event(ev);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyWnd w;
    w.show();

    return a.exec();
}

#include "main.moc"
