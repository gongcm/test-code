#include <QtGui/QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <assert.h>
#include "word.h"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    QString msg, demostr="hello world word 2003\r";
    QVariant filename, start , end;
    Word::Application *docapp=NULL;
    Word::Range *wptr=NULL;
    int pos = 0;

    //Create a new Document
    Word::Document *doc = new Word::Document();
    if(!doc){
        msg = "Fail to Document()";
        goto ERR_STEP1;
    }
    qDebug()<< "TRACE:" << __FUNCTION__<< ":" << __LINE__ << endl;

    //get Document instance
    docapp = doc->Application();
    if(!docapp){
        msg = "Fail to Application()";
        goto ERR_STEP2;
    }
    docapp->SetVisible(false);
    docapp->SetDisplayAlerts(Word::wdAlertsNone);

    qDebug()<< "RET: " << doc->Name() << endl;

    for(int i = 0; i < 3; i++){
        start = QVariant(pos+i*demostr.length());
        end = QVariant(pos+(i+1)*demostr.length());
        wptr = doc->Range(start, end);
        if(!wptr){
            msg = "Fail to Range()";
            goto ERR_STEP2;
        }
        wptr->SetText(demostr);
    }

    wptr = doc->Range();
    if(!wptr){
        msg = "Fail to Range()";
        goto ERR_STEP2;
    }
    msg = wptr->Text();
    pos = msg.length();
    qDebug() << "LEN = " << pos;
    qDebug() << "CONTENT: ";
    qDebug() << msg << endl;

    filename = QVariant(QDir::currentPath()+"/demo.doc");
    doc->SaveAs(filename);

    doc->Close();
    docapp->Quit();
    msg = "Well done.";

ERR_STEP2:
    delete doc;

ERR_STEP1:
    QMessageBox::warning(NULL, QString("Warning"), msg);
    return 0;
}
