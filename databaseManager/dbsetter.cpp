#include "dbsetter.h"

DbSetter::DbSetter()
{

}

DbSetter::~DbSetter(){
        QSqlDatabase::removeDatabase("QSQLITE");
}


// public methods: add delete update logic

int DbSetter::addTransaction(const Transaction& t){

    if(t.isFuture()) return true;

    if(t.isIncome()){
        if(t.fromTotalSavings())
            updateAccountValue("Total Savings",t.getAmount());
        else{
            double spare = get.getAccountValue("Default Savings")-get.getAccountValue("Savings");
            if(spare != 0){
                if(t.getAmount()>spare){
                    updateAccountValue("Savings",spare);
                    updateAccountValue("Budget",t.getAmount()-spare);
                }
                else
                    updateAccountValue("Savings",t.getAmount());
            }
            else
                updateAccountValue("Budget",t.getAmount());
        }
        updateAccountValue(t.getType(),t.getAmount());
        updateIncomesValue(t.getCategory(),t.getAmount(),t.getDate());
        return createTransaction(t);
    }

    if(t.isFromPrevMonth() || t.fromTotalSavings())
    {
        if(t.getAmount()<=get.getAccountValue("Total Savings")){
            updateAccountValue("Total Savings",-t.getAmount());
            updateAccountValue(t.getType(),t.getAmount());
            updateExpensesValue(t.getCategory(),t.getAmount(),t.getDate());
            return createTransaction(t); }
        else
            return -1;
    }
    else
    {
        if(t.getAmount() <= get.getAccountValue("Budget")){
            updateAccountValue("Budget",-t.getAmount());
            updateAccountValue(t.getType(),t.getAmount());
            updateExpensesValue(t.getCategory(),t.getAmount(),t.getDate());
            return createTransaction(t);
        }
        else if(t.getAmount() <= (get.getAccountValue("Budget")+get.getAccountValue("Savings")))
        {
            updateAccountValue("Savings",(get.getAccountValue("Budget")-t.getAmount()));
            setAccountValue("Budget",0);
            updateAccountValue(t.getType(),t.getAmount());
            updateExpensesValue(t.getCategory(),t.getAmount(),t.getDate());
            return createTransaction(t);
        }
        else
            return -1;
    }

}

int DbSetter::updateTransaction(const Transaction& t){

    Transaction backup = get.getTransactions(t.getID());

    if(t.isExpense()){
        int i = deleteTransaction(t.getID());
        if(i==1) {
            int newId = addTransaction(t);
            if(newId == -1){
                addTransaction(backup);
                return -1;
            }
            else return newId;
        }
        else return -1;
    }

    else { // t is income
        int newId = addTransaction(t);
        if(newId != -1){
            int i = deleteTransaction(t.getID());
            if(i==-1){
                deleteTransaction(newId);
                return -1;
            }

            else return newId;
        }
        else return -1; // what to do here
    }
}

int DbSetter::deleteTransaction(int id){

    // set value negative for income, positive for expenses
    Transaction t = get.getTransactions(id);

    if(t.isExpense()){
        if(t.fromTotalSavings())
            updateAccountValue("Total Savings",t.getAmount());
        else{
            double spare = get.getAccountValue("Default Savings")-get.getAccountValue("Savings");
            if(spare != 0){
                if(t.getAmount()>spare){
                    updateAccountValue("Savings",spare);
                    updateAccountValue("Budget",t.getAmount()-spare);
                }
                else
                    updateAccountValue("Savings",t.getAmount());
            }
            else
                updateAccountValue("Budget",t.getAmount());
        }
        updateAccountValue(t.getType(),-t.getAmount());
        updateExpensesValue(t.getCategory(),-t.getAmount(),t.getDate());
        removeTransaction(t.getID());
        return 1;
    }

    bool oldTransaction = t.getDate().month() < IPDate.month()
            && t.getDate().year() <= IPDate.year();

    if(oldTransaction || t.fromTotalSavings())
    {
        if(t.getAmount()<=get.getAccountValue("Total Savings")){
            updateAccountValue("Total Savings",-t.getAmount());
            updateIncomesValue(t.getCategory(),-t.getAmount(),t.getDate());
            removeTransaction(t.getID());
            return 1; }
        else
            return -1;
    }
    else
    {
        if(t.getAmount() <= get.getAccountValue("Budget")){
            updateAccountValue("Budget",-t.getAmount());
            updateAccountValue(t.getType(),-t.getAmount());
            updateIncomesValue(t.getCategory(),-t.getAmount(),t.getDate());
            removeTransaction(t.getID());
            return 1;
        }
        else if(t.getAmount() <= (get.getAccountValue("Budget")+get.getAccountValue("Savings")))
        {
            updateAccountValue("Savings",(get.getAccountValue("Budget")-t.getAmount()));
            setAccountValue("Budget",0);
            updateAccountValue(t.getType(),-t.getAmount());
            updateIncomesValue(t.getCategory(),-t.getAmount(),t.getDate());
            removeTransaction(t.getID());
            return 1;
        }
        else
            return -1;
    }

}

bool DbSetter::transferAmount(QString from, QString to, double amount){

    if(get.getAccountValue(from)<amount) return false;

    updateAccountValue(to, amount);
    updateAccountValue(from,-amount);

    return true;
}

void DbSetter::scheduleTransaction(const Transaction& t){
    if(!t.isRecurring() && !t.isFuture()) return;

    QDate newDate {};

    if(t.isFuture())
        newDate = t.getDate();
    else if(t.getRecurringRange()=="Days")
        newDate = t.getDate().addDays(t.getRecurringValue());
    else if(t.getRecurringRange()=="Weeks")
        newDate = t.getDate().addDays(7*t.getRecurringValue());
    else if(t.getRecurringRange()=="Months")
        newDate = t.getDate().addMonths(t.getRecurringValue());
    else if(t.getRecurringRange()=="Years")
        newDate = t.getDate().addYears(t.getRecurringValue());

    QSqlQuery query;
    query.prepare("INSERT INTO tblSchedule "
                  "(type, category, amount, day, month, year, date, description, account, recurringValue, recurringRange) "
                  "VALUES "
                  "(:type, :category, :amount, :day, :month, :year, :date,  :description, :account, :recurringValue, :recurringRange)");
    query.bindValue(":type",t.getType());
    query.bindValue(":category",t.getCategory());
    query.bindValue(":amount",t.getAmount());
    query.bindValue(":day", newDate.day());
    query.bindValue(":month",newDate.month());
    query.bindValue(":year",newDate.year());
    query.bindValue(":date",newDate);
    query.bindValue(":description",t.getDesc());
    query.bindValue(":account",t.getAccount());
    query.bindValue(":recurringValue",t.getRecurringValue());
    query.bindValue(":recurringRange",t.getRecurringRange());
    query.exec();
}

void DbSetter::updateSchedule(const Transaction& t){

    QDate newDate {};

    if(t.getRecurringValue()==0){
        deleteSchedule(t);
        return;
    }

    if(t.isFuture())
        newDate = t.getDate();
    else if(t.getRecurringRange()=="Days")
        newDate = t.getDate().addDays(t.getRecurringValue());
    else if(t.getRecurringRange()=="Weeks")
        newDate = t.getDate().addDays(7*t.getRecurringValue());
    else if(t.getRecurringRange()=="Months")
        newDate = t.getDate().addMonths(t.getRecurringValue());
    else if(t.getRecurringRange()=="Years")
        newDate = t.getDate().addYears(t.getRecurringValue());

    QSqlQuery query;
    query.prepare("UPDATE tblSchedule SET day = :day, month = :month, year = :year, date = :date "
                  "WHERE "
                  "(ids = :id)");
    query.bindValue(":id", t.getID());
    query.bindValue(":day", newDate.day());
    query.bindValue(":month",newDate.month());
    query.bindValue(":year",newDate.year());
    query.bindValue(":date",newDate);
    query.exec();
}

void DbSetter::deleteSchedule(const Transaction& t){
    QSqlQuery query;
    query.prepare("DELETE from tblSchedule WHERE ids = :id");
    query.bindValue(":id",t.getID());
    query.exec();
}

void DbSetter::updateAccount(const double income, const double savings, const double totalSavings){
    setAccountValue("Default Income",income);
    setAccountValue("Default Savings",savings);
    setAccountValue("Total Savings",totalSavings);
}


// private methods: Create database entries

void DbSetter::createExpensesKey(const QString key, const double value, QDate date){
    QSqlQuery query;
    query.prepare("INSERT INTO tblExpenses "
                  "(month,year,key,value) "
                  "VALUES (:month,:year,:key,:value)");
    query.bindValue(":month",date.month());
    query.bindValue(":year",date.year());
    query.bindValue(":key",key);
    query.bindValue(":value",value);
    query.exec();
}

void DbSetter::createIncomesKey (const QString key, const double value, QDate date){
QSqlQuery query;
    query.prepare("INSERT INTO tblIncomes "
                  "(month,year,key,value) "
                  "VALUES (:month,:year,:key,:value)");
    query.bindValue(":month",date.month());
    query.bindValue(":year",date.year());
    query.bindValue(":key",key);
    query.bindValue(":value",value);
    query.exec();
}

void DbSetter::addToHistory(const QString key, const double value, QDate date){
    QSqlQuery query;
        query.prepare("INSERT INTO tblHistory "
                      "(month,year,key,value) "
                      "VALUES (:month,:year,:key,:value)");
        query.bindValue(":month",date.month());
        query.bindValue(":year",date.year());
        query.bindValue(":key",key);
        query.bindValue(":value",value);
        query.exec();
}

// private methods: Set / update / delete database entries

void DbSetter::updateAccountValue (const QString key,const double value){
    double update = get.getAccountValue(key) + value;
    setAccountValue(key,update);
}

void DbSetter::setAccountValue (const QString key, const double value){
    QSqlQuery query;
    query.prepare("UPDATE tblAccount SET value = :value WHERE key = :key");
    query.bindValue(":key", key);
    query.bindValue(":value", value);
    query.exec();
}

void DbSetter::updateExpensesValue (const QString key, const double value, const QDate date){

    double update = get.getExpensesValue(key,date) + value;
    QSqlQuery query;
    query.prepare("UPDATE tblExpenses SET value = :update "
                  "WHERE "
                  "(key = :key "
                  "AND month = :month "
                  "AND year = :year)");
    query.bindValue(":key", key);
    query.bindValue(":update", update);
    query.bindValue(":month",date.month());
    query.bindValue(":year",date.year());
    query.exec();
}

void DbSetter::updateIncomesValue (const QString key, const double value, const QDate date){

    double update = get.getIncomesValue(key,date) + value;
    QSqlQuery query;
    query.prepare("UPDATE tblIncomes"
                  " SET value = :update "
                  "WHERE "
                  "(key = :key "
                  "AND month = :month "
                  "AND year = :year)");
    query.bindValue(":key", key);
    query.bindValue(":update", update);
    query.bindValue(":month",date.month());
    query.bindValue(":year",date.year());
    query.exec();
}


// private methods: transactions and operations

int DbSetter::createTransaction (const Transaction& t){
    QSqlQuery query;
    query.prepare("INSERT INTO tblTransactions "
                  "(type, category, amount, day, month, year, date, description, account) "
                  "VALUES "
                  "(:type, :category, :amount, :day, :month, :year, :date,  :description, :account)");
    query.bindValue(":type",t.getType());
    query.bindValue(":category",t.getCategory());
    query.bindValue(":amount",t.getAmount());
    query.bindValue(":day", t.getDay());
    query.bindValue(":month",t.getMonth());
    query.bindValue(":year",t.getYear());
    query.bindValue(":date",t.getDate());
    query.bindValue(":description",t.getDesc());
    query.bindValue(":account",t.getAccount());
    query.exec();

    return query.lastInsertId().toInt();

    // add operation
}

void DbSetter::removeTransaction (const int id){
    QSqlQuery query;
    query.prepare("DELETE from tblTransactions WHERE ids = :id");
    query.bindValue(":id",id);
    query.exec();
}

void DbSetter::addOperation (Operation& o){
    QSqlQuery query;
    query.prepare("INSERT INTO tblOperations "
                  "(day, month, year, type, amount, description) "
                  "VALUES "
                  "(:day, :month, :year, :type, :amount, :description)");
    query.bindValue(":type",o.getType());
    query.bindValue(":amount",o.getAmount());
    query.bindValue(":day", o.getDay());
    query.bindValue(":month",o.getMonth());
    query.bindValue(":year",o.getYear());
    query.bindValue(":description",o.getDesc());
    query.exec();
}
