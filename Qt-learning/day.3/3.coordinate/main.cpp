#include <QApplication>
#include <QDialog>
#include <QDesktopWidget>
#include <QScreen>
#include <QLabel>
#include <QRect>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDialog *wnd = new QDialog;
    wnd->setMaximumSize(320, 240);
    wnd->setMinimumSize(320, 240);

    QVBoxLayout *vlayout = new QVBoxLayout;
    QLabel *label = new QLabel;

    QDesktopWidget *desk = a.desktop();
    QString text;
    text.sprintf("Desktop: X=%d, Y=%d; W=%d, H=%d", desk->x(), desk->y(),desk->width(), desk->height());
    label->setText(text);
    label->setAlignment(Qt::AlignCenter);
    vlayout->addWidget(label);

    QScreen *screen = a.screens().at(0);
    label = new QLabel;
    QRect rect = screen->geometry();
    text.sprintf("Screen: X=%d, Y=%d; W=%d, H=%d", rect.x(), rect.y(), rect.width(), rect.height());
    label->setText(text);
    label->setAlignment(Qt::AlignCenter);
    vlayout->addWidget(label);

    label = new QLabel;
    rect = wnd->geometry();
    text.sprintf("Widget: X=%d, Y=%d; W=%d, H=%d", rect.x(), rect.y(), rect.width(), rect.height());
    label->setText(text);
    label->setAlignment(Qt::AlignCenter);
    vlayout->addWidget(label);

    label = new QLabel;
    rect = wnd->frameGeometry();
    text.sprintf("frameGeometry: X=%d, Y=%d; W=%d, H=%d", rect.x(), rect.y(), rect.width(), rect.height());
    label->setText(text);
    label->setAlignment(Qt::AlignCenter);
    vlayout->addWidget(label);

    wnd->setLayout(vlayout);
    wnd->setWindowTitle(QString("Demo"));
    wnd->show();

    return a.exec();
}
