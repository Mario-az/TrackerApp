#ifndef OPERATION_H
#define OPERATION_H

#include <QString>
#include <QDate>

class Operation
{
private:
    QDate date;
    QString type;
    double amount;
    QString description;
public:
    int getDay() const {return date.day();}
    int getMonth() const {return date.month();}
    int getYear() const {return date.year();}
    int getAmount() const {return amount;}
    QDate getDate() const {return date;}
    QString getType() const {return type;}
    QString getDesc() const {return description;}

    void setDate(QDate a){date =a;}
    void setDate(int y, int m, int d){date.setDate(y,m,d);}
    void setAmount(int a){amount =a;}
    void setType(QString a){type =a;}
    void setDesc(QString a){description =a;}

};

#endif // OPERATION_H
