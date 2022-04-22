#ifndef ADDTRANSACTIONFORM_H
#define ADDTRANSACTIONFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QVector>
#include "templates/transaction.h"
#include "databaseManager/global.h"
#include "databaseManager/dbgetter.h"
#include "databaseManager/dbsetter.h"

namespace Ui {
class AddTransactionForm;
}

class AddTransactionForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddTransactionForm(QWidget *parent, QString name);
    ~AddTransactionForm();

private:
    Ui::AddTransactionForm *ui;
    QString name;
    const QDate IPDate = Global::currentDate();
    QStringList expensesCategories;
    QStringList incomeCategories;
    DbGetter get = DbGetter();
    DbSetter set = DbSetter();

private:
    void initializeWidget();
    void resetLabels();
    bool inputValid() const;
    void successMessage(bool,QString);
    void loadSchedule();
    QWidget* createWidgetSchedule(Transaction& t);

private slots:
    void SubmitBtnClicked();

signals:
    void appRefresh();
};

#endif // ADDTRANSACTIONFORM_H
