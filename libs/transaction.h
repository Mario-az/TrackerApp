#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <vector>
#include <string>
#include <QString>
#include <iostream>
#include <QDate>
#include "libs/Utilities/global.h"

class Transaction
{
private:
    int id;
    QString type;
    QString category;
    QString DbCategory;
    double amount;
    QString description;
    QDate date;
    QString account;
    int day;
    int month;
    int year;

    // schedule variables
    bool recurring;
    int recurringValue;
    QString recurringRange;

public:
    Transaction() = default;
    ~Transaction() = default;

//    Transaction &operator=(Transaction&& other);
    int     getID()             const   {return id;};
    QString getType()           const   {return type;};
    QString getCategory()       const   {return category;};
    double  getAmount()         const   {return amount;};
    QString getDesc()           const   {return description;};
    int     getDay()            const   {return day;}
    int     getMonth()          const   {return month;}
    int     getYear()           const   {return year;}
    QDate   getDate()           const   {return date;}
    QString getAccount()        const   {return account;}
    bool    isRecurring()       const   {return recurring;}
    int     getRecurringValue() const   {return recurringValue;}
    QString getRecurringRange() const   {return recurringRange;}
    bool    isExpense()         const   {return this->getType()=="Expenses";}
    bool    isIncome()          const   {return this->getType()=="Income";}
    bool    fromTotalSavings()  const   {return this->getAccount() == "Total Savings";}
    bool    isFuture()          const   {return this->getDate() > Global::currentDate();}
    bool    isFromPrevMonth()   const   {return this->getDate().month() < Global::currentDate().month()
                                             && this->getDate().year() <= Global::currentDate().year();}

    void    setID       (int a)         {id = a;};
    void    setType     (QString a)     {type = a;};
    void    setAmount   (double a)      {amount = a;};
    void    setDesc     (QString a)     {description = a;};
    void    setCategory (QString a)     { category = a; };
    void    setDay      (int a)         {day = a;}
    void    setMonth    (int a)         {month = a;}
    void    setYear     (int a)         {year = a;}
    void    setAccount  (QString a)     {account = a;}
    void    setDate     (QDate a)       {date = a;
                                         day = a.day();
                                         month = a.month();
                                         year = a.year();}

    void    setRecurring      (bool a)         {recurring = a;}
    void    setRecurringValue   (int a)        {recurringValue = a;}
    void    setRecurringRange   (QString a)    {recurringRange = a;}


};

#endif // TRANSACTION_H
