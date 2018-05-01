#ifndef FIRSTQTCLASS_H
#define FIRSTQTCLASS_H

#include <QWidget>

class QPushButton;
class QString;

namespace Ui {
class FirstQtClass;
}

class FirstQtClass : public QWidget
{
    Q_OBJECT

public:
    explicit FirstQtClass(QWidget *parent = 0);
    ~FirstQtClass();
signals:
    void sigDemo(QString, int);
public slots:
    void slotBtn();
    void slotDemo(QString);

private:
    Ui::FirstQtClass *ui;
    QPushButton *btn;
};

#endif // FIRSTQTCLASS_H
