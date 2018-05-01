#include <QCoreApplication>
#include <QtTest/QtTest>

class TestQString: public QObject
{
    Q_OBJECT
private slots:
    void toUpper();
};

#if 0
void TestQString::toUpper()
{
    QString str = "Hello";
    QVERIFY(str.toUpper() == "HELLO");
}
#else
void TestQString::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO1"));
}
#endif

#if 0
int main(int argc, char *argv[])
{
    TestQString obj;
    QTest::qExec(&obj, argc, argv);
}
#else
QTEST_MAIN(TestQString)
#endif

#include "main.moc"
