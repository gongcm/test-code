#include <QApplication>
#include <QLibrary>

typedef void (*GETOBJ)(void);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLibrary lib("DLL.dll");
    if(!lib.load()){
        qDebug("Load DLL.dll fail.\n");
        return -1;
    }

    GETOBJ pfun = (GETOBJ)lib.resolve("getobject");
    if(NULL != pfun){
        pfun();
    }else{
        qDebug("symbol error.\n");
        return -1;
    }

    return a.exec();
}

