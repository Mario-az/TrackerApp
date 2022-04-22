#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QWidget>
#include <QValidator>
#include <QDir>
#include <QStandardPaths>
#include "trackerapp.h"
#include "databaseManager/global.h"
#include "databaseManager/dbconnection.h"

namespace Ui {
class StartScreen;
class AccountsList;
}

class StartScreen : public QWidget
{
    Q_OBJECT

public:
    explicit StartScreen(QWidget *parent = nullptr);
    ~StartScreen();

private:
    Ui::StartScreen *ui;
    QStringList accountsList;
    QStringList getAccounts();
    const QDate IPDate = Global::currentDate();

private:
    void displayAccounts();
    void clearLabels();
    bool checkDuplicate(QString name);

private slots:
    void startTrackerApp();
    void newAccountBtn();
    void createAccount();
    void backToAccounts();
    void setSavingsLimit();
    void setCurrency(int a);
};

#endif // STARTSCREEN_H
