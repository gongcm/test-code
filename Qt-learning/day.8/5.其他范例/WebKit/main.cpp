#include <QApplication>
#include <QWebView>
#include <QUrl>
#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>
#include <QMouseEvent>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWebView wnd;
    QUrl url("http://www.baidu.com");
    wnd.load(url);

    QWebFrame* frame = wnd.page()->currentFrame();
    if (NULL != frame)
    {
        QPoint pos = wnd.topLevelWidget()->mapToGlobal(QPoint(0,0));

        QMouseEvent pressev(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(wnd.page(), &pressev);

        QMouseEvent releaseev(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(wnd.page(), &releaseev);
    }

    wnd.show();

    return a.exec();
}
