#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include <QApplication>
#include <QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDate>
#include <QFile>

class Global : public QObject
{
    Q_OBJECT

friend class StartScreen;

public:
    explicit Global(QObject *parent = nullptr);
    static QDate    currentDate();
    static QString  toCurrency(double);
    static QString  CurrencySymbol();

private:
    static QDate IPDate;
    static QString curr;
    bool success();
    static void setCurrency(QString);
};

#endif // GLOBAL_H
