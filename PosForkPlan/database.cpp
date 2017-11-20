#include "database.h"
#include <QMessageBox>
#include <QSqlDatabase>

database::database()
{

}

void database::OpenDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(QString("DRIVER={SQL SERVER};"
                               "SERVER=%1;"
                               "DATABASE=%2;"
                               "UID=%3;"
                               "PWD=%4;")
                       .arg("115.159.110.221,9528")
                       .arg("Freemud_PosUpgrade")
                       .arg("waimaitest")
                       .arg("waimaitest@freemud"));
     if (!db.open())
     {
         QMessageBox::warning(0, qApp->tr("Cannot open database"),
                  db.lastError().databaseText(), QMessageBox::Cancel);
     }
     else
     {
         qDebug()<<"Connect to Database Success!";
     }
}

void database::ChangeDatabase(QString address,QString databaseName,QString uid,QString pwd)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(QString("DRIVER={SQL SERVER};"
                               "SERVER=%1;"
                               "DATABASE=%2;"
                               "UID=%3;"
                               "PWD=%4;")
                       .arg(address)
                       .arg(databaseName)
                       .arg(uid)
                       .arg(pwd));

     if (!db.open())
     {
         QMessageBox::warning(0, qApp->tr("Cannot open database"),
                  db.lastError().databaseText(), QMessageBox::Cancel);
     }
     else
     {
         qDebug()<<"Connect to Database Success!";
     }
}


