#include <QApplication>
#include <QTranslator>

#include "mainwidget.h"

int main(int argc, char * argv[])
{
    QApplication app(argc,argv);

    QFont font("宋体",12);
    app.setFont(font);
    
    MainWidget w;
    w.show();

    return app.exec();
}
