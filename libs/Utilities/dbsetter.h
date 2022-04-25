#ifndef DBSETTER_H
#define DBSETTER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <clocale>
#include <cctype>
#include <QDate>
#include <QStandardPaths>
#include "libs/transaction.h"
#include "libs/Utilities/global.h"
#include "libs/Utilities/dbgetter.h"
#include "libs/operation.h"

class DbSetter
{
public:
    friend class DbConnection;

    DbSetter();
    ~DbSetter();
    int addTransaction      (const Transaction& t);
    int updateTransaction   (const Transaction& t);
    int deleteTransaction   (const int id);
    void addOperation       (Operation& o);
    bool transferAmount     (QString from, QString to, double amount);

    void scheduleTransaction(const Transaction& t);
    void updateSchedule     (const Transaction& t);
    void deleteSchedule     (const Transaction& t);

    void updateAccount      (const double, const double, const double);

private:
    int createTransaction   (const Transaction& t);
    void removeTransaction  (const int id);

    void updateAccountValue (const QString key, const double value);
    void setAccountValue    (const QString key, const double value);

    void updateExpensesValue    (const QString key, const double value, const QDate date);
    void updateIncomesValue     (const QString key, const double value, const QDate date);

    void createExpensesKey  (const QString key, const double value, QDate date);
    void createIncomesKey   (const QString key, const double value, QDate date);

    void addToHistory       (const QString key, const double value, QDate date);


    DbGetter get = DbGetter();
    const QDate IPDate = Global::currentDate();
};

#endif // DBSETTER_H
