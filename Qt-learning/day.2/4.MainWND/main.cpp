#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    QFont f("宋体", 12);
    QApplication::setFont(f);

    MainWindow *mainwindow = new MainWindow();
    mainwindow->show();

    return app.exec();
}
