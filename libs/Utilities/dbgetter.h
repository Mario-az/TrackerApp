#ifndef DBGETTER_H
#define DBGETTER_H

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
#include "libs/operation.h"

class DbGetter
{
public:
    DbGetter();
    double getAccountValue  (const QString key) const;
    double getExpensesValue (const QString key,QDate date) const;
    double getIncomesValue  (const QString key,QDate date) const;
    QList<Transaction> getTransactions (QDate date) const;
    QList<Transaction> getSchedule     (QDate date) const;
    QList<Transaction> getSchedule     () const;
    Transaction getTransactions        (const int id) const;
    QStringList getIncomesCategories   () const;
    QStringList getExpensesCategories  () const;
    QList<Operation> getOperations () const;

private:
    const QDate IPDate = Global::currentDate();
    enum transactionKeys
        { id,
          type,
          category,
          amount,
          day,
          month,
          year,
          date,
          description,
          account,
          recurringValue,
          recurringRange    };
    QStringList expensesCategories
        { "Housing",
          "Transport.",
          "Food",
          "Utilities",
          "Insurance",
          "Medical",
          "Investment",
          "Personal",
          "Other"           };
    QStringList incomesCategories
        { "Salary",
          "Bonus",
          "Freelance",
          "Investment",
          "Rental Prop.",
          "Other" };

};

#endif // DBGETTER_H
