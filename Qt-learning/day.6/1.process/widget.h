#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected slots:
    void slotstart();
    void slotkill();
    void slotterminate();
    void slotstarted();
    void slotdestroyed();
    void slotfinished(int);

private:
    Ui::Widget *ui;
    QProcess *process;
};

#endif // WIDGET_H
