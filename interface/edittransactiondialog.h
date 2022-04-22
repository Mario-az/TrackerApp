#ifndef EDITTRANSACTIONDIALOG_H
#define EDITTRANSACTIONDIALOG_H

#include <QDialog>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include "templates/transaction.h"
#include "databaseManager/global.h"
#include "databaseManager/dbgetter.h"
#include "databaseManager/dbsetter.h"

namespace Ui {
class editTransactionDialog;
}

class editTransactionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit editTransactionDialog(QWidget *parent, int id);
    ~editTransactionDialog();

private:
    Ui::editTransactionDialog *ui;
    Transaction t;
    DbGetter get = DbGetter();
    DbSetter set = DbSetter();
    const QDate IPDate = Global::currentDate();
    QStringList expensesCategories;
    QStringList incomeCategories;
              
private:
    void successMessage(bool);
    void initialize();
    void addStyle();
    void successMessage(bool,QString);
    bool inputValid() const;

private slots:
    void updateBtnClicked();
    void optionToggled(bool enabled);

signals:
    void transactionUpdate();
};

#endif // EDITTRANSACTIONDIALOG_H
