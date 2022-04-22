#ifndef TRACKERAPP_H
#define TRACKERAPP_H

#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include "interface/addtransactionform.h"
#include "interface/edittransactiondialog.h"
#include "databaseManager/global.h"
#include "databaseManager/dbgetter.h"
#include "databaseManager/dbconnection.h"
#include "databaseManager/dbsetter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TrackerApp; }
QT_END_NAMESPACE

class TrackerApp : public QWidget
{
    Q_OBJECT

public:
    TrackerApp(QWidget *parent, QString accountName);
    ~TrackerApp();

private:
    Ui::TrackerApp *ui;
    QString name;
    DbGetter get = DbGetter();
    DbSetter set = DbSetter();
    const QDate IPDate = Global::currentDate();

private:
    void initApp();
    void loadExpensesCategories();
    void loadIncomesCategories();
    void connectBtns();
    void initSettings();
    void successMessage(bool a, QString message);

    QWidget* createWidgetTransaction(Transaction& t);
    QWidget* createWidgetSchedule(Transaction& t);

private slots:
    void refresh();
    void loadTransactions();
    void transferAmount();

};


#endif // TRACKERAPP_H
