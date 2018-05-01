#include <QApplication>
#include <QTextCodec>

#include "tcpserver.h"

int main( int argc, char **argv )
{
    QApplication app(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK2312"));

    TcpServer tcpserver;
    tcpserver.show();

    return app.exec();
}
