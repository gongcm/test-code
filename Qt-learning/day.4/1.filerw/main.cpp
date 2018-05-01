#include <QCoreApplication>
#include <QFile>
#include <QByteArray>
#include <QDebug>

#include <QTextStream>
#include <QDataStream>
#include <QDate>

#include <stdio.h>

//#define FILEIOTEST
//#define TEXTSTREAM
#define DATASTREAM

#ifdef FILEIOTEST
void demoforfilerw(char *filename)
{
    QString text;
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly)){
        QByteArray context = file.readAll();
        text = "READ: "+context;
        file.close();
    }else{
        text = "file open: "+file.errorString();
    }

    QFile out;
    if(out.open(stdout, QIODevice::WriteOnly)){
        out.write(text.toStdString().data());
        out.write("\r\n");
        out.close();
    }else{
        qDebug() << "stdout open: "  << out.errorString() << endl;
    }
}

void demoforcreatedfile(char *filename)
{
    QByteArray context;
    context = "1234567890";

    QFile file(filename);
    //if(file.open(QIODevice::ReadWrite | QIODevice::Append)){
    if(file.open(QIODevice::ReadWrite | QIODevice::Truncate)){
        file.write(context);
        file.close();
    }else{
        qDebug() << "file open: "  << file.errorString() << endl;
    }
}
#endif

#ifdef TEXTSTREAM
void demoforfilerw(char *filename)
{
    QString text;
    QString usr;
    QString name;
    QString passwd;
    QString num;

    QFile file(filename);
    if(file.open(QIODevice::ReadOnly)){
        QTextStream out(&file);
        out >> usr >> name >> passwd >> num;
        qDebug() << "READ: ";
        qDebug() << usr ;
        qDebug() << name ;
        qDebug() << passwd ;
        qDebug() << num ;
        file.close();
     }else{
        qDebug() << "file open: "  << file.errorString() << endl;
        return;
    }
}

void demoforcreatedfile(char *filename)
{
    QFile file(filename);
    //if(file.open(QIODevice::ReadWrite | QIODevice::Append)){
    if(file.open(QIODevice::ReadWrite | QIODevice::Truncate)){
        QTextStream out(&file);
        QString usr("user");
        QString name("demo");
        QString passwd("passwd");
        QString num("123456789");
        out << usr << endl;
        out << name << endl;
        out << passwd << endl;
        out << num << endl;
        file.close();
    }else{
        qDebug() << "file open: "  << file.errorString() << endl;
    }
}
#endif

#ifdef DATASTREAM
void demoforfilerw(char *filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly)){
        QDataStream out(&file);
        int i = 0;
        while(!out.atEnd()){
            qDebug("READ[%d]: ", i++);

            QDate date;
            QString name;
            qint64 passwd;
            out >> date >> name >> passwd ;

            qDebug() << date.toString() << endl;
            qDebug() << name << endl;
            qDebug("%ld", passwd);
        }
        file.close();
    }else{
        qDebug() << "file open: " << file.errorString() << endl;
    }
}

void demoforcreatedfile(char *filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadWrite | QIODevice::Append)){
        QDataStream out(&file);
        out << QDate::fromString("1MM12car2003", "d'MM'MMcaryyyy").currentDate();
        out << QString("passwd");
        out << (qint64)1234567890;
        file.close();
    }else{
        qDebug() << "file open: "  << file.errorString() << endl;
    }
}
#endif

int main(int argc, char *argv[])
{
    qDebug() << "Start ...." << endl;
    char filename[] = "1.dat";
    demoforcreatedfile(filename);
    demoforfilerw(filename);
    qDebug() << "demoforfilerw done." << endl;
}
