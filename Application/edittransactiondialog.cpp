#include "edittransactiondialog.h"
#include "ui_edittransactiondialog.h"

editTransactionDialog::editTransactionDialog(QWidget *parent, int id):
    QDialog(parent),
    ui(new Ui::editTransactionDialog)
{
    ui->setupUi(this);
    t = get.getTransactions(id);
    addStyle();
    initialize();
}

editTransactionDialog::~editTransactionDialog()
{
    delete ui;
}

void editTransactionDialog::addStyle(){
    // style the window borderless rounded corners
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    const QRect rect(QPoint(0,0), this->geometry().size());
    QBitmap b(rect.size());
    b.fill(QColor(Qt::color0));
    QPainter painter(&b);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(rect, 12, 12, Qt::AbsoluteSize);
    painter.end();
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(100);
    effect->setOffset(0, 0);
    this->setGraphicsEffect(effect);
    this->setMask(b);
}

void editTransactionDialog::initialize(){
    expensesCategories = get.getExpensesCategories();
    incomeCategories = get.getIncomesCategories();

    ui->transDateBox->setDate(t.getDate());
    ui->transDescBox->setMaxLength(16);
    ui->transDescBox->setText(t.getDesc());
    ui->transAmntBox->setValue(t.getAmount());
    ui->transDescBox->setMaxLength(16);
    ui->transDateBox->setMaximumDate(QDate(IPDate.year(),IPDate.month(),
                                           IPDate.daysInMonth()));
    ui->transReportLabels->setCurrentIndex(0);
    ui->transAmntBox->setPrefix(Global::CurrencySymbol() + " ");

    connect(ui->closeBtn, &QPushButton::released, this,
        [this](){ close();});

    // Initialize RadioButton switch
    connect(ui->optIncome, SIGNAL(toggled(bool)), this, SLOT(optionToggled(bool)));
    connect(ui->optExpenses, SIGNAL(toggled(bool)), this, SLOT(optionToggled(bool)));
    if(t.getType()=="Income") ui->optIncome->toggle();
    if(t.getType()=="Expenses") ui->optExpenses->toggle();

    // Initialize Submit Button signal
    connect(ui->transUpdateBtn, SIGNAL(clicked()), this, SLOT(updateBtnClicked()));

    // Initialize Delete Button signal
    connect(ui->deleteButton, &QPushButton::clicked, this, [this](){

    double tempSavings = get.getAccountValue("Savings");
    int result = set.deleteTransaction(t.getID());

    switch(result){

    case -1:
        successMessage(false, "Budget not enough.");
        break;

    default:
        double removed = tempSavings - get.getAccountValue("Savings");
        if(removed!=0)
            successMessage(true, Global::toCurrency(removed) + " removed from savings.");
        else
            successMessage(true, "Transaction added successfully.");

        emit emit this->transactionUpdate();
        close();
        break;

    }

    });
}

bool editTransactionDialog::inputValid() const {
    if (ui->transDescBox->text().isEmpty()
            ||ui->transCatBox->currentText().isEmpty()
            ||ui->transAmntBox->value() == 0)
         return false;
    else return true;
};

void editTransactionDialog::optionToggled(bool){
    ui->transCatBox->clear();
    if(ui->optIncome->isChecked())
        ui->transCatBox->addItems(incomeCategories);
    if(ui->optExpenses->isChecked())
        ui->transCatBox->addItems(expensesCategories);
    ui->transCatBox->setCurrentText(t.getCategory());
}

void editTransactionDialog::updateBtnClicked(){
    if(! inputValid()) {
        successMessage(false, "Input is not valid.");
        return;
    }

    QDate date = ui->transDateBox->date();
    QString desc = ui->transDescBox->text();
    double amount = ui->transAmntBox->value();
    QString cat = ui->transCatBox->currentText();
    QString type = ui->optIncome->isChecked()? "Income" : "Expenses";

    t.setDate(date);
    t.setAmount(amount);
    t.setType(type);
    t.setCategory(cat);
    t.setDesc(desc);

    double tempSavings = get.getAccountValue("Savings");

    int result = set.updateTransaction(t);

    switch(result){

    case -1:
        successMessage(false, "Budget not enough.");
        break;

    default:
        double removed = tempSavings - get.getAccountValue("Savings");
        if(removed!=0)
            successMessage(true, Global::toCurrency(removed) + " removed from savings.");
        else
            successMessage(true, "Transaction added successfully.");

        emit this->transactionUpdate();

        t = get.getTransactions(result);

        break;
    }
}

void editTransactionDialog::successMessage(bool success,QString text){
    if(success){
        ui->transReportLabels->setCurrentIndex(1);
        ui->transSuccessLabel->setText(text);
    }
    else {
        ui->transReportLabels->setCurrentIndex(2);
        ui->transErrorLabel->setText(text);
    }
}

