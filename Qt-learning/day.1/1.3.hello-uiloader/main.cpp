#include <QApplication>
#include <QUiLoader>
#include <QWidget>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUiLoader loader;
    QFile file(":/firstwnd.ui");
    file.open(QFile::ReadOnly);

    QWidget wnd;
    QWidget *myWidget = loader.load(&file, &wnd);
    file.close();

    wnd.show();

    return app.exec();
}
