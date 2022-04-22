#include "global.h"
#include <QDir>

QDate Global::IPDate = QDate::currentDate();
QString Global::curr = "w";

Global::Global(QObject *parent): QObject{parent}{

    // inquire API for date from ip. avoid wrong system clock.

    // read token from file
    QFile file("token.txt");
    QString token;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            token = stream.readLine();
        }
    }
    file.close();


    // start event loop
    QEventLoop eventLoop;


    // create a network request and wait for reply
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest request;
    request.setUrl(QUrl("https://timezoneapi.io/api/ip/?token="+token));
    QNetworkReply *reply = manager->get(request);

    // exit loop upon reply
    connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    // read reply and set to static variable of class
    QString strReply = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(strReply.toUtf8());
    QJsonObject object = document.object();

    auto dateString = document["data"]
                      .toObject()["datetime"]
                      .toObject()["date"]
                      .toString();
    IPDate = QDate::fromString(dateString,"MM/dd/yyyy");
}

QString Global::toCurrency(const double x){

    // format variable to currency qstring
    double z = abs(x);
    double dec = z-floor(z);
    if(dec!=0) z = z - dec;
    QString result = QString::number(z,'f',2);
    int y = QString::number(z,'f',0).size();
    int w = y % 3;
    if(w==0) w = 3;
    if (y>3){
        for(int i = w; i <= y;i=i+4)
        {
        result.insert(i,',');
        }
    }
    if(x<0) result = "-" + result;
    result = CurrencySymbol() + result;
    if(dec!=0){
        result.chop(3);
        result.push_back(QString::number(dec,'f',2).remove(0,1));
    }
    return result;
}

QString Global::CurrencySymbol(){
    return curr;
}

QDate Global::currentDate(){
    return IPDate;
}

void Global::setCurrency(QString a){
    curr = a;
}
