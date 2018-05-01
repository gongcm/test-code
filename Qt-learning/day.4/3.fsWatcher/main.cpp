#include "fileviewer.h"
#include <QApplication>
#include <QDebug>
#include <QFont>
#include <QDesktopWidget>


int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    QFont font("宋体", 12);
    QApplication::setFont(font);

    FileViewer *fileviewer = new FileViewer;
    fileviewer->show();

    return a.exec();
}
