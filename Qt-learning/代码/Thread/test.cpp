#include "test.h"
#include <QDebug>

Test::Test(QObject *parent) : QObject(parent)
{

}

void Test::testSLot()
{
    qDebug() << "testSLot()";
}
