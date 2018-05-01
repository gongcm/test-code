#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QNetworkReply>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void analysisJsonData(QByteArray &jsonData);

public slots:
    void handlerNetworkReply(QNetworkReply *networkReply);
    void handlerNetworkError(QNetworkReply::NetworkError errorCode);

private slots:
    void on_lineEdit_returnPressed();

private:
    Ui::Widget *ui;
    QNetworkAccessManager *networkManager ;
};

#endif // WIDGET_H
