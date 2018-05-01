#include <QCoreApplication>
#include <QSqlDatabase>
#include <QStringList>
#include <QDebug>
  
int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
      
    qDebug() << "Available drivers:";
    QStringList drivers = QSqlDatabase::drivers();
    foreach(QString driver, drivers){ 
        qDebug() << "\t" << driver;
	}

#ifdef Q_OS_LINUX
    aaaa
#endif

        #ifdef Q_OS_WIN
            aaaa
        #endif

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    qDebug() << "ODBC driver valid ? " << db.isValid();
}
