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

    this->installEventFilter(this);
}

MyWnd::~MyWnd()
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;

    label->deleteLater();
    delete hlayout;
}

bool MyWnd::event(QEvent * ev)
{
    QString text;

    switch (ev->type()) {
    case QEvent::KeyPress:
        text.sprintf("keyPressEvent: %d", ((QKeyEvent *)ev)->key());
        label->setText(text);
        break;

    case QEvent::KeyRelease:
        text.sprintf("keyReleaseEvent: %d", ((QKeyEvent *)ev)->key());
        label->setText(text);
        break;

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
        break;

    case QEvent::MouseMove:
        text.sprintf("mouseMoveEvent: %d,%d; %d,%d", ((QMouseEvent*)ev)->pos().x(), ((QMouseEvent*)ev)->pos().y(),
                     ((QMouseEvent*)ev)->globalPos().x(), ((QMouseEvent*)ev)->globalPos().y());
        label->setText(text);
        break;

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
        break;

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
        break;

    case QEvent::Move:
        text.sprintf("moveEvent: %d,%d; %d,%d", ((QMoveEvent*)ev)->pos().x(), ((QMoveEvent*)ev)->pos().y(),
                     ((QMoveEvent*)ev)->oldPos().x(), ((QMoveEvent*)ev)->oldPos().y());
        label->setText(text);
        break;

    default:
        break;
    }

    return QWidget::event(ev);
}

//#define FILTERDONE

bool MyWnd::eventFilter(QObject *obj, QEvent *event)
{
    QString text;

    switch (event->type()) {
    case QEvent::KeyPress:
        text.sprintf("keyPressEvent: %d", ((QKeyEvent *)event)->key());
        qDebug() << text << endl;
#ifdef FILTERDONE
        return true;
#else
        break;
#endif
    case QEvent::KeyRelease:
        text.sprintf("keyReleaseEvent: %d", ((QKeyEvent *)event)->key());
        qDebug() << text << endl;
#ifdef FILTERDONE
        return true;
#else
        break;
#endif

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
#ifdef FILTERDONE
        return true;
#else
        break;
#endif

    case QEvent::MouseMove:
        text.sprintf("mouseMoveEvent: %d,%d; %d,%d", ((QMouseEvent*)event)->pos().x(), ((QMouseEvent*)event)->pos().y(),
                     ((QMouseEvent*)event)->globalPos().x(), ((QMouseEvent*)event)->globalPos().y());

        qDebug() << text << endl;
#ifdef FILTERDONE
        return true;
#else
        break;
#endif

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
#ifdef FILTERDONE
        return true;
#else
        break;
#endif

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
#ifdef FILTERDONE
        return true;
#else
        break;
#endif

    case QEvent::Move:
        text.sprintf("moveEvent: %d,%d; %d,%d", ((QMoveEvent*)event)->pos().x(), ((QMoveEvent*)event)->pos().y(),
                     ((QMoveEvent*)event)->oldPos().x(), ((QMoveEvent*)event)->oldPos().y());
        qDebug() << text << endl;
#ifdef FILTERDONE
        return true;
#else
        break;
#endif

    default:
        break;
    };

    return QWidget::eventFilter(obj, event);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyWnd w;
    w.show();

    return a.exec();
}

#include "main.moc"
