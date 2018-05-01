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
	void keyPressEvent(QKeyEvent * event);
	void keyReleaseEvent(QKeyEvent * event);
	void mouseDoubleClickEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void moveEvent(QMoveEvent * event);

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
}

void MyWnd::keyPressEvent(QKeyEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;

    QString text;
    text.sprintf("keyPressEvent: %d", event->key());
    qDebug() << text << endl;
	text += ";"+event->text();
    label->setText(text);
}

void MyWnd::keyReleaseEvent(QKeyEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;
    QString text;
    text.sprintf("keyReleaseEvent: %d", event->key());
    qDebug() << text << endl;
	text += ";"+event->text();
    label->setText(text);
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
    label->setText("mouseDoubleClickEvent:"+text);
}

void MyWnd::mouseMoveEvent(QMouseEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;
    QString text;
    text.sprintf("mouseMoveEvent: %d,%d; %d,%d", event->pos().x(), event->pos().y(),
                 event->globalPos().x(), event->globalPos().y());
    qDebug() << text << endl;
    label->setText(text);
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
    label->setText("mousePressEvent:"+text);
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
    label->setText("mouseReleaseEvent:"+text);
}

void MyWnd::moveEvent(QMoveEvent * event)
{
    qDebug() << "TRACE:" << __FUNCTION__ << __LINE__ << endl;
    QString text;
    text.sprintf("moveEvent: %d,%d; %d,%d", event->pos().x(), event->pos().y(),
                 event->oldPos().x(), event->oldPos().y());
    qDebug() << text << endl;
    label->setText(text);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyWnd w;
    w.show();

    return a.exec();
}

#include "main.moc"
