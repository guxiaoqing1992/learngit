#ifndef DATABASE_H
#define DATABASE_H

#include <QtGui>
#include <QString>
#include <QTextCodec>
#include <QSqlDatabase>
#include <QtSql>

class database
{
public:
    database();
    void OpenDatabase();
    void ChangeDatabase(QString,QString,QString,QString);
};

#endif // DATABASE_H
