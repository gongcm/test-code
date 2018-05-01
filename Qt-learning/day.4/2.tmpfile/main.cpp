#include <QCoreApplication>
#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>

#include <QDebug>
#include <assert.h>

#include <stdio.h>

class InvalidPtrException {
public:
    InvalidPtrException(QTemporaryFile *tmpf){
        qDebug("%s:tmpf addr : 0x%p", __func__, tmpf);
    }
};
void isvalid(QTemporaryFile *tmpf) throw(InvalidPtrException)
{
     if(NULL == tmpf)
        throw InvalidPtrException(tmpf);
}

class FailOpenException {
public:
    FailOpenException(QTemporaryFile *tmpf){
        qDebug() << "Open Fail: " << tmpf->errorString() << endl;
    }
};
void isfail(QTemporaryFile *tmpf) throw (FailOpenException)
{
    if(!tmpf->open())
        throw FailOpenException(tmpf);

//    tmpf->setAutoRemove(false);
    tmpf->seek(tmpf->size());
}

void writetotmpf(QTemporaryFile *tmpf)
{
    qDebug() << "File: " << tmpf->fileName() << endl;
    QTextStream out(tmpf);
    qDebug() << "write tmp done." << endl;
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    try {
#if 1
        QFile f(":/xxx");
        QTemporaryFile *tmpf = QTemporaryFile::createLocalFile(f);
        //QTemporaryFile *tmpf = QTemporaryFile::createNativeFile(f);

        isvalid(tmpf);
        isfail(tmpf);
        writetotmpf(tmpf);
#else
        QTemporaryFile tmpf;

        isvalid(&tmpf);
        isfail(&tmpf);
        writetotmpf(&tmpf);
#endif
    } catch (InvalidPtrException) {
        qDebug() << "tmpf Is a Invalid pointer";
    } catch (FailOpenException) {
        qDebug() << "fail to open tmpf";
    } catch (...) {
        qDebug() << "-------------------------";
    }

    return app.exec();
}
