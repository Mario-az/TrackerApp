#include "dbconnection.h"

// create and remove database connection

DbConnection::DbConnection(QString userName): name{userName}
{
    // check if QSQLITE is available
    if(!QSqlDatabase::isDriverAvailable("QSQLITE")){
        qDebug() << "sqlite not available";
        return;
    };

    // create or connect to database of user
    accountData = QSqlDatabase::addDatabase("QSQLITE");
    QString loc = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    loc.append( "/TrackerApp data/");
    loc.append(userName);
    loc.append(".db");
    accountData.setDatabaseName(loc);
    accountData.open();
}

DbConnection::~DbConnection(){}


// initialize new database

void DbConnection::initAccount(double income, double savings, double totalSavings, QString curr)
{

    initTables();

    // user stats
    createAccountKey("Income",0);
    createAccountKey("Savings",savings);
    createAccountKey("Budget",-savings);
    createAccountKey("Expenses",0);
    createAccountKey("Total Savings",totalSavings);
    createAccountKey("Default Income",income);
    createAccountKey("Default Savings",savings);
    createAccountKey("Login Month", IPDate.month());
    createAccountKey("Login Year", IPDate.year());
    createAccountKey(curr, -1);

    // default exp categories
    for(auto &r: get.getExpensesCategories()){
        set.createExpensesKey(r, 0, IPDate);
    }

    // default inc categories
    for(auto &r: get.getIncomesCategories()){
        set.createIncomesKey(r, 0, IPDate);
    }

    // add salary to beginning of month
    if(income !=0){
        QDate date = IPDate.addDays(1-IPDate.day());
        salaryEntry(date);
    }

}

void DbConnection::initTables()
{
    accountData.open();

    // to hold defaults and current month values
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS tblAccount"
                  "(key TEXT, value REAL)");
    query.exec();
    query.clear();

    // to hold old months values INCOME EXPENSES SAVINGS TOTAL SAVINGS
    query.prepare("CREATE TABLE IF NOT EXISTS tblHistory"
                  "(month INT, year INT, key TEXT, value REAL)");
    query.exec();
    query.clear();

    // to hold current and old months incomes categories values
    query.prepare("CREATE TABLE IF NOT EXISTS tblIncomes"
                  "(month INT, year INT, key TEXT, value REAL)");
    query.exec();
    query.clear();

    // to hold current and old months expenses categories values
    query.prepare("CREATE TABLE IF NOT EXISTS tblExpenses"
                  "(month INT, year INT, key TEXT, value REAL)");
    query.exec();
    query.clear();

    // to hold all transactions added
    query.prepare("CREATE TABLE IF NOT EXISTS tblTransactions"
                  " (ids INTEGER NOT NULL PRIMARY KEY, "
                  "type TEXT, "
                  "category TEXT, "
                  "amount REAL, "
                  "day INT, "
                  "month INT, "
                  "year INT, "
                  "date REAL, "
                  "description TEXT, "
                  "account TEXT)");
    query.exec();
    query.clear();

    // to hold all scheduled transactions
    query.prepare("CREATE TABLE IF NOT EXISTS tblSchedule"
                  " (ids INTEGER NOT NULL PRIMARY KEY, "
                  "type TEXT, "
                  "category TEXT, "
                  "amount REAL, "
                  "day INT, "
                  "month INT, "
                  "year INT, "
                  "date REAL, "
                  "description TEXT, "
                  "account TEXT, "
                  "recurringValue INT, "
                  "recurringRange TEXT)");
    query.exec();
    query.clear();

    // add operations widget
    query.prepare("CREATE TABLE IF NOT EXISTS tblOperations"
                  " (ids INTEGER NOT NULL PRIMARY KEY, "
                  "day INT, "
                  "month INT, "
                  "year INT, "
                  "type TEXT, "
                  "amount REAL, "
                  "description TEXT)");
    query.exec();
    query.clear();

}

void DbConnection::createAccountKey (const QString type, const double value){
    // insert key into main account
    QSqlQuery query;
    query.prepare("INSERT INTO tblAccount "
                  "(key,value) VALUES (:type,:value)");
    query.bindValue(":type",type);
    query.bindValue(":value",value);
    query.exec();
}

QString DbConnection::getCurrency() {
    // get default set user currency
    QSqlQuery query;
    query.prepare("SELECT * FROM tblAccount WHERE value = :value");
    query.bindValue(":value",-1);
    query.exec();
    query.next();
    QString value = query.value(0).toString();
    return value;
}


// reset database at beginning of month

void DbConnection::monthlyReset() {

    // check difference between last login and current date
    double monthDiff = IPDate.month() - get.getAccountValue("Login Month")
            + 12*(IPDate.year() - get.getAccountValue("Login Year"));

    // if same month exit
    if (monthDiff == 0) return;

    // if month is different, loop for each month
    // add expenses and savings to first month, else income to other months.
    for(int i = monthDiff; i>0;--i){
        if(i==monthDiff){
                    set.setAccountValue("Total Savings",
                                        get.getAccountValue("Total Savings") +
                                        get.getAccountValue("Budget") +
                                        get.getAccountValue("Savings"));
            }
        else{
            set.setAccountValue("Total Savings",
                                get.getAccountValue("Total Savings") +
                                get.getAccountValue("Default Income"));
            }
        resetKeys();
        createCategories(IPDate.addMonths(-i+1));
        set.addToHistory("Total Savings",get.getAccountValue("Total Savings"),IPDate.addMonths(-i+1));

        if(get.getAccountValue("Default Income") !=0){
            QDate date = IPDate.addDays(1-IPDate.day()).addMonths(-i+1);
            salaryEntry(date);
            }

        // create expenses categories for every month, to keep database coherent
        createCategories(IPDate);
    }
}

void DbConnection::resetKeys(){

    // reset keys at month beginning
    set.setAccountValue("Budget",-get.getAccountValue("Default Savings"));
    set.setAccountValue("Income",0);
    set.setAccountValue("Expenses",0);
    set.setAccountValue("Savings",get.getAccountValue("Default Savings"));
    set.setAccountValue("Login Month", IPDate.month());
    set.setAccountValue("Login Year", IPDate.year());
}

void DbConnection::createCategories(QDate date){

    // create categories for each month of the year
    for(auto &r: get.getExpensesCategories()){
        set.createExpensesKey(r, 0, date);
        }

    for(auto &r: get.getIncomesCategories()){
        set.createIncomesKey(r, 0, date);
        }
}

void DbConnection::salaryEntry(QDate date){

    // enter salary at beginning of month
    Transaction t;
    t.setAccount("Budget");
    t.setType("Income");
    t.setCategory("Salary");
    t.setDesc("Monthly Salary");
    t.setAmount(get.getAccountValue("Default Income"));
    t.setDate(date);
    set.addTransaction(t);
}
