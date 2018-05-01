#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSharedMemory>

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
    void slotrdmem();
    void slotwrmem();

private:
    Ui::Widget *ui;
    QSharedMemory *sharemem;
};

#endif // WIDGET_H
