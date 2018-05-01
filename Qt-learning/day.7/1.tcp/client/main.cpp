#include <QApplication>
#include <QTextCodec>

#include "tcpclient.h"

int main( int argc, char **argv )
{
    QApplication app(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK2312"));

    TcpClient *tcpclient = new TcpClient();
    tcpclient->show();

    return app.exec();
}
