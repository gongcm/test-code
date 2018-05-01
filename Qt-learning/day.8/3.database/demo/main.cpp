#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>
  
int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    if(!db.isValid()){
        qDebug() << "ERR:" << db.lastError().text();
        return -1;
    }

    db.setDatabaseName("demo.db");

    if(!db.open()){
        qDebug() << db.lastError().text();
        return -1;
    }

    QSqlQuery query;

    if(!query.exec("CREATE TABLE IF NOT EXISTS demo (id INT, name VARCHAR(24 ) NOT NULL, age INT)")){
        goto ERR_STEP;
    }

    if(!query.exec("DELETE FROM demo")){
        goto ERR_STEP;
    }

    for(int i = 0; i < 6; i++){
        std::string namestr[6] ={
            "jane",
            "jack",
            "Mike",
            "Bill",
            "Jobs",
            "Henry" };
        QString sql;
        sql.sprintf("INSERT INTO demo (id, name, age) VALUES (%d, '%s', %d)", i, namestr[i].data(), qrand()%30+20);
        if(!query.exec(sql)){
            goto ERR_STEP;
        }
    }

#if 1
    if(!query.exec("SELECT id,name,age FROM demo")){
        goto ERR_STEP;
    }
#else
    if(!query.exec("SELECT * FROM demo")){
        goto ERR_STEP;
    }
#endif

    while (query.next()) {
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        QString age = query.value(2).toString();
        qDebug() << "ID: " << id << "; name: " << name << "; age: " << age;
    }

    db.close();
    return 0;

ERR_STEP:
    qDebug() << "exec fail:" << query.lastError().text();
    db.close();
    return -1;
}
