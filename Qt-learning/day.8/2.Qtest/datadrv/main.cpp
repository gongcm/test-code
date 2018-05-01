#include <QCoreApplication>
#include <QtTest/QtTest>

class TestQString: public QObject
{
    Q_OBJECT
private slots:
    void toUpper();
    void toUpper_data();
};

void TestQString::toUpper_data()
{
    QTest::addColumn<QString>("src");
    QTest::addColumn<QString>("dst");

    QTest::newRow("all lower") << "hello" << "HELLO";
    QTest::newRow("mixed")     << "Hello" << "HELLO";
    QTest::newRow("all upper") << "HELLO" << "HELLO";
    QTest::newRow("fix ") << "HeLlO" << "HELLO";
}

void TestQString::toUpper()
{
    QFETCH(QString, src);
    QFETCH(QString, dst);

    QCOMPARE(src.toUpper(), dst);
}

QTEST_MAIN(TestQString)

#include "main.moc"
