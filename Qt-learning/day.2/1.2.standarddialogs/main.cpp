#include "standarddialogs.h"
#include <QApplication>
#include <QFont>
#include <QTranslator>
#include <QApplication>


int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    QFont font("宋体",16);
    QApplication::setFont(font);
    
    StandardDialogs *standarddialogs = new StandardDialogs();
    standarddialogs->show();
    return a.exec();
}
