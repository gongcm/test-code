#include "inputdialog.h"
#include <QApplication>
#include <QFont>
#include <QTranslator>
#include <QDialog>

int main( int argc, char **argv )
{ 	
    QApplication app( argc, argv );

    QFont font("·ÂËÎ",12);
    QApplication::setFont(font);

    InputDlg *input = new InputDlg();
    input->show();


    return app.exec();
}
