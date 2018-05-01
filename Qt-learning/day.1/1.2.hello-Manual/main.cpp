#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QLayout>

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    QLabel *label;
};

#define W 320
#define H 240

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    label = new QLabel(this);
    label->setText(QString("Hello Qt World"));
    label->setAlignment(Qt::AlignCenter);

    this->setMaximumSize(W,H);
    this->setMinimumSize(W,H);

    label->move(W/4,H/4);
    label->setMaximumSize(W/2,H/2);
    label->setMinimumSize(W/2,H/2);

    label->show();
}

Widget::~Widget()
{
    label->deleteLater();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.show();

    return a.exec();
}

#include "main.moc"
