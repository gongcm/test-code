#include <QtWidgets>
#include <QPushButton>
#include <QtTest/QtTest>

#if 0
class Mybtn : public QPushButton
{
    Q_OBJECT
public:
    explicit Mybtn(QWidget *parent = 0):
       QPushButton(parent)
    {
        QObject::connect(this, SIGNAL(clicked()), this, SLOT(slotsetbtn()));
    }
    ~Mybtn(){}

private slots:
    void slotsetbtn()
    {
        if(QString("Ok") == text()){
            setText(QString("Cancel"));
            qDebug("-----------1-------------");
        }else{
            setText(QString("Ok"));
            qDebug("-----------2-------------");
        }
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Mybtn *btn = new Mybtn;
    btn->setText(QString("Ok"));
    btn->show();

    QTest::keyClick(btn, Qt::Key_Space);
    QTest::mouseClick(btn, Qt::LeftButton);

    return app.exec();
}

#else
class Mybtn : public QPushButton
{
    Q_OBJECT
public:
    explicit Mybtn(QWidget *parent = 0):
       QPushButton(parent)
    {
        QObject::connect(this, SIGNAL(clicked()), this, SLOT(slotsetbtn()));
    }
    ~Mybtn(){}
#if 0
public:
#else
private:
#endif
    int test(int i ){
        int *p = 0;
        *p = i;
        return i;
    }

private slots:
    void slotsetbtn()
    {
        if(QString("Ok") == text()){
            setText(QString("Cancel"));
            qDebug("-----------1-------------");
        }else{
            setText(QString("Ok"));
            qDebug("-----------2-------------");
        }
    }
};

class TestGui: public QObject
{
    Q_OBJECT
private slots:
    void testGui();
};

void TestGui::testGui()
{
    Mybtn btn;
    btn.setText("Ok");
    QTest::keyClick(&btn, Qt::Key_Space);
    QCOMPARE(btn.text(), QString("Cancel"));

   qDebug() << "TARCE：" << btn.text();

    QTest::keyClick(&btn, Qt::Key_Space);
    QCOMPARE(btn.text(), QString("Ok"));
}

QTEST_MAIN(TestGui)
#endif

#include "main.moc"
