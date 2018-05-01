/**/
#include <QApplication>
#include <QtGui>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QPushButton>

#define BTNMAX 5

class LayoutDemo : public QWidget
{
    Q_OBJECT

public:
    explicit LayoutDemo(QWidget *parent = 0);
    ~LayoutDemo();

private:
    QGridLayout *gLayout;
    QHBoxLayout *hLayout;
    QVBoxLayout *vLayout;
    QPushButton *btn[BTNMAX];
};

LayoutDemo::LayoutDemo(QWidget *parent):
    QWidget(parent)
{
    for(int i = 0; i < BTNMAX; i++){
        QString text;
        text.sprintf("BTN&%d", i);
        btn[i] = new QPushButton(text);
    }

    gLayout = new QGridLayout;
    hLayout = new QHBoxLayout;
    vLayout = new QVBoxLayout;

    hLayout->addWidget(btn[0]);
    hLayout->addStretch();
    hLayout->addWidget(btn[1]);
    hLayout->addStretch();
    hLayout->addWidget(btn[2]);

    vLayout->addWidget(btn[3]);
    vLayout->addStretch();
    vLayout->addWidget(btn[4]);

    gLayout->addWidget(btn[0], 0, 0);
 //   gLayout->addLayout(btn[0], 0, 0);
    gLayout->addLayout(vLayout, 1, 1);

    this->setLayout(gLayout);
}

LayoutDemo::~LayoutDemo()
{
    for(int i = 0; i < BTNMAX; i++){
        btn[i]->deleteLater();
    }

    delete hLayout;
    delete vLayout;
    delete gLayout;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LayoutDemo w;
    w.show();

    return a.exec();
}

#include "main.moc"
