#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class QFile;
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_inputButton_clicked();

    void on_showButton_clicked();

    void on_fileBrowse_clicked();

private:
    Ui::Widget *ui;
    QString filePath;
};

#endif // WIDGET_H
