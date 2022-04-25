#include "dbgetter.h"

DbGetter::DbGetter(){

}

double DbGetter::getAccountValue(const QString key) const{
    QSqlQuery query;
    query.prepare("SELECT * FROM tblAccount WHERE key = :key");
    query.bindValue(":key",key);
    query.exec();
    query.next();
    double value = query.value(1).toDouble();
    return value;
}

double DbGetter::getExpensesValue(const QString key,const QDate date) const{
    QSqlQuery query;
    query.prepare("SELECT * FROM tblExpenses WHERE key = :key "
                  "AND month = :month AND year = :year");
    query.bindValue(":key",key);
    query.bindValue(":month",date.month());
    query.bindValue(":year",date.year());
    query.exec();
    query.next();
    double value = query.value(3).toDouble();
    return value;
}

double DbGetter::getIncomesValue(const QString key,QDate date) const{
    QSqlQuery query;
    query.prepare("SELECT * FROM tblIncomes WHERE key = :key "
                  "AND month = :month AND year = :year");
    query.bindValue(":key",key);
    query.bindValue(":month",date.month());
    query.bindValue(":year",date.year());
    query.exec();
    query.next();
    double value = query.value(3).toDouble();
    return value;
}

QList<Transaction> DbGetter::getTransactions(const QDate date) const{

    // get all transaction with same date
    QList<Transaction> result {};
    QSqlQuery query;
    query.prepare("SELECT * FROM tblTransactions WHERE date = :date");
    query.bindValue(":date",date);
    query.exec();

    // push transaction into QLIST
    Transaction t = Transaction();
    while (query.next()) {
        t.setID(query.value(id).toInt());
        t.setType(query.value(type).toString());
        t.setCategory(query.value(category).toString());
        t.setAmount(query.value(amount).toDouble());
        t.setDate(date);
        t.setDesc(query.value(description).toString());

        result.push_back(t);
    }
    return result;
}

Transaction DbGetter::getTransactions(const int id) const{
    QSqlQuery query;
    query.prepare("SELECT * FROM tblTransactions WHERE ids = :id");
    query.bindValue(":id",id);
    query.exec();
    query.next();
    Transaction t = Transaction();

    t.setID(query.value(0).toInt());
    t.setType(query.value(type).toString());
    t.setCategory(query.value(category).toString());
    t.setAmount(query.value(amount).toDouble());
    t.setDay(query.value(day).toInt());
    t.setMonth(query.value(month).toInt());
    t.setYear(query.value(year).toInt());
    t.setDate(query.value(date).toDate());
    t.setDesc(query.value(description).toString());
    t.setAccount(query.value(account).toString());
    return t;
}

QStringList DbGetter::getExpensesCategories() const{
    return expensesCategories;
}

QStringList DbGetter::getIncomesCategories() const{
    return incomesCategories;
}

QList<Operation> DbGetter::getOperations() const{

    QList<Operation> result {};

    QSqlQuery query;
    query.prepare("SELECT * from tblOperations");
    query.exec();
    Operation o = Operation();
    while (query.next()) {
        o.setDate(query.value(3).toInt(),
                  query.value(2).toInt(),
                  query.value(1).toInt());

        o.setType(query.value(4).toString());
        o.setAmount(query.value(5).toDouble());
        o.setDesc(query.value(6).toString());

        result.push_back(o);
    }

    return result;
}

QList<Transaction> DbGetter::getSchedule() const{
    QList<Transaction> result {};
    QSqlQuery query;
    query.prepare("SELECT * FROM tblSchedule");
    query.exec();

    Transaction t = Transaction();
    while (query.next()) {
        t.setID(query.value(id).toInt());
        t.setType(query.value(type).toString());
        t.setCategory(query.value(category).toString());
        t.setAmount(query.value(amount).toDouble());
        t.setDate(query.value(date).toDate());
        t.setDesc(query.value(description).toString());
        t.setRecurring(true);
        t.setRecurringValue(query.value(recurringValue).toInt());
        t.setRecurringRange(query.value(recurringRange).toString());
        result.push_back(t);
    }
    return result;
}

QList<Transaction> DbGetter::getSchedule(const QDate date) const{

    QList<Transaction> result {};
    QSqlQuery query;
    query.prepare("SELECT * FROM tblSchedule WHERE day <= :day AND month <= :month AND year <= :year");
    query.bindValue(":day",date.day());
    query.bindValue(":month",date.month());
    query.bindValue(":year",date.year());
    query.exec();

    Transaction t = Transaction();
    while (query.next()) {
        t.setID(query.value(id).toInt());
        t.setType(query.value(type).toString());
        t.setCategory(query.value(category).toString());
        t.setAmount(query.value(amount).toDouble());
        t.setDate(date);
        t.setDesc(query.value(description).toString());
        t.setRecurring(true);
        t.setRecurringValue(query.value(recurringValue).toInt());
        t.setRecurringRange(query.value(recurringRange).toString());

        result.push_back(t);
    }
    return result;
}

