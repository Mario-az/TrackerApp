#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <clocale>
#include <cctype>
#include <QDate>
#include <QStandardPaths>
#include "templates/transaction.h"
#include "global.h"
#include "dbgetter.h"
#include "dbsetter.h"

class DbConnection
{
public:
    DbConnection(QString dbName);
    ~DbConnection();
    void initAccount    (double, double, double, QString);
    void monthlyReset   ();
    QString getCurrency ();

private:
    void initTables();
    void createAccountKey(const QString type,const  double value);
    void resetKeys();
    void createCategories(QDate);
    void salaryEntry(QDate date);

private:
    QString name;
    QSqlDatabase accountData;
    DbGetter get = DbGetter();
    DbSetter set = DbSetter();
    const QDate IPDate = Global::currentDate();


};

#endif // DBCONNECTION_H
