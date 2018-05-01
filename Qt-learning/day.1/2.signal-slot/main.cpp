#include "firstqtclass.h"
#include <QApplication>
#include <QtDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FirstQtClass w;
    w.show();

    return a.exec();
}
