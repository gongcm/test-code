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
	bool eventFilter(QObject *obj, QEvent *event);

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

    this->installEventFilter(this);
}

MyWnd::~MyWnd()
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;

    label->deleteLater();
    delete hlayout;
}

bool MyWnd::eventFilter(QObject *obj, QEvent *event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;
    QString text;

    switch (event->type()) {
    case QEvent::KeyPress:
        text.sprintf("keyPressEvent: %d", ((QKeyEvent *)event)->key());
        qDebug() << text << endl;
        label->setText(text);
        return true;

    case QEvent::KeyRelease:
        text.sprintf("keyReleaseEvent: %d", ((QKeyEvent *)event)->key());
        qDebug() << text << endl;
        label->setText(text);
        return true;

    case QEvent::MouseButtonDblClick:
        switch(((QMouseEvent *)event)->button()){
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
        label->setText("mouseDoubleClickEvent:"+text);
        return true;

    case QEvent::MouseMove:
        text.sprintf("mouseMoveEvent: %d,%d; %d,%d", ((QMouseEvent*)event)->pos().x(), ((QMouseEvent*)event)->pos().y(),
                     ((QMouseEvent*)event)->globalPos().x(), ((QMouseEvent*)event)->globalPos().y());
        qDebug() << text << endl;
        label->setText(text);
        return true;

    case QEvent::MouseButtonPress:
        switch(((QMouseEvent *)event)->button()){
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
        label->setText("mousePressEvent:"+text);
        return true;

    case QEvent::MouseButtonRelease:
        switch(((QMouseEvent *)event)->button()){
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
        label->setText("mouseReleaseEvent:"+text);
        return true;

    case QEvent::Move:
        text.sprintf("moveEvent: %d,%d; %d,%d", ((QMoveEvent*)event)->pos().x(), ((QMoveEvent*)event)->pos().y(),
                     ((QMoveEvent*)event)->oldPos().x(), ((QMoveEvent*)event)->oldPos().y());
        qDebug() << text << endl;
        label->setText(text);
        return true;

    default:
        //qDebug() << "default: the other event" << endl;
        //label->setText("default: the other event");
        break;
    }

//    return false;
    return QObject::eventFilter(obj, event);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyWnd w;
    w.show();

    return a.exec();
}

#include "main.moc"
