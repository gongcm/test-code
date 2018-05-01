#include <QtGui/QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QTextEdit>
#include <QString>
#include <QPrinter>
#include <assert.h>

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    //save to pdf
    QTextEdit *textEdit = new QTextEdit(NULL);
    textEdit->setGeometry(0,0,800,480);
    textEdit->setText("helloworld");
    QString fileName = QFileDialog::getSaveFileName(NULL,"Export PDF",QString(),"*.pdf");
    if(!fileName.isEmpty()){
        if(QFileInfo(fileName).suffix().isEmpty()){
            fileName.append(".pdf");
        }

        QPrinter  printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        textEdit->document()->print(&printer);
    }

    return app.exec();
}
