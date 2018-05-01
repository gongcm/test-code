#include "widget.h"
#include "ui_widget.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(handlerNetworkReply(QNetworkReply*)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::analysisJsonData(QByteArray &jsonData)
{
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(jsonData,&jsonError);

    if(!document.isNull() && jsonError.error == QJsonParseError::NoError){
        QJsonObject object = document.object();
        QJsonObject resultObject = object.value(QString("result")).toObject();
        QJsonArray  songsJsonArray = resultObject.value(QString("songs")).toArray();

        for(int i = 0;i < songsJsonArray.count(); i ++){
            QJsonObject jsonObject = songsJsonArray[i].toObject();
            //获取的专辑的信息
            QJsonObject albumObject = jsonObject["album"].toObject();
            QString albumName = albumObject.value(QString("name")).toString();
            QString picUrl    = albumObject.value(QString("picUrl")).toString();

            //艺术家的信息
            QJsonArray artistsArray = jsonObject.value(QString("artists")).toArray();
            QString artistsName = artistsArray[0].toObject().value("name").toString();

            //音频信息
            QString audioUrl = jsonObject.value(QString("audio")).toString();
            qint32  songId   = jsonObject.value(QString("id")).toInt();
            QString songName = jsonObject.value(QString("name")).toString();


            qDebug() << "albumName  : " << albumName;
            qDebug() << " picUrl    : " <<  picUrl;
            qDebug() << "artistsName: " << artistsName;
            qDebug() << "audioUrl   : " << audioUrl;
            qDebug() << "songId     : " << songId;
            qDebug() << "songName   : " << songName;

            qDebug() << "--------------------------------------------------------";

        }
    }
}

void Widget::handlerNetworkReply(QNetworkReply *networkReply)
{
    if(networkReply->error() == QNetworkReply::NoError){
         QByteArray data = networkReply->readAll();
         analysisJsonData(data);
    }
}

void Widget::handlerNetworkError(QNetworkReply::NetworkError errCode)
{
    qDebug() << "network error : " << errCode;
}

void Widget::on_lineEdit_returnPressed()
{
    qDebug() << ui->lineEdit->text();
    //运算符重载
    QString encodeString = ui->lineEdit->text().toUtf8().toPercentEncoding();
    QString httpUrl =  QString("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%1&limit=20").arg(encodeString);
    qDebug() << "httpUrl : " << httpUrl;

    QNetworkReply *networkReply = networkManager->get(QNetworkRequest(QUrl(httpUrl)));
    connect(networkReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(handlerNetworkError(QNetworkReply::NetworkError)));
    connect(networkReply,SIGNAL(finished()),networkReply,SLOT(deleteLater()));

    ui->lineEdit->clear();
}
