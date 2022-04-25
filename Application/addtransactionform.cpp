#include "addtransactionform.h"
#include "ui_addtransactionform.h"

AddTransactionForm::AddTransactionForm(QWidget *parent, QString accountName) :
    QWidget(parent),
    ui(new Ui::AddTransactionForm),
    name(accountName)
{
    ui->setupUi(this);
    expensesCategories = get.getExpensesCategories();
    incomeCategories = get.getIncomesCategories();
    initializeWidget();
    loadSchedule();
}

AddTransactionForm::~AddTransactionForm()
{
    delete ui;
}

void AddTransactionForm::initializeWidget(){

    // labels and boxes
    ui->transDateBox->setDate(IPDate);
    ui->transReportLabels->setCurrentIndex(0);
    ui->transAmntBox->setValue(0);
    ui->transDescBox->setMaxLength(16);
    ui->stackedWidgets->setCurrentIndex(0);
    ui->accountBox->addItems({"Current Budget", "Total Savings"});
    ui->accountBox->setCurrentIndex(0);
    ui->transAmntBox->setPrefix(Global::CurrencySymbol() + " ");
    ui->recurringRange->setDisabled(true);
    ui->recurringValue->setDisabled(true);
    ui->recurringRange->addItems({"Days","Weeks","Months","Years"});

    // initialize date selection
    connect(ui->transDateBox,&QDateEdit::dateChanged,this,[this](QDate date){
        if(date > Global::currentDate())
             ui->transSubmitBtn->setText(" Schedule Transaction");
        else ui->transSubmitBtn->setText(" Submit Transaction");
    });


    // Initialize RadioButton switch
    connect(ui->optExpenses, &QRadioButton::toggled, this, [this](bool a){
        ui->transCatBox->clear();
        if(a) ui->transCatBox->addItems(expensesCategories); });

    connect(ui->optIncome, &QRadioButton::toggled, this, [this](bool a){
        ui->transCatBox->clear();
        if(a) ui->transCatBox->addItems(incomeCategories); });


    // Initialize Submit Button signal
    connect(ui->transSubmitBtn, SIGNAL(clicked()), this, SLOT(SubmitBtnClicked()));


    // Initialize info btn
    connect(ui->infoBtn, &QPushButton::toggled, this, [this](bool checked){
        if      (checked)   ui->stackedWidgets->setCurrentIndex(1);
        else if (!checked)  ui->stackedWidgets->setCurrentIndex(0);
         });

    // recurring checkbox
    connect(ui->recurringCheckBox,&QCheckBox::toggled,this,[this](bool a){
        if(a){
            ui->recurringRange->setDisabled(false);
            ui->recurringValue->setDisabled(false);
        }
        else{
            ui->recurringRange->setDisabled(true);
            ui->recurringValue->setDisabled(true);
        }
    });

}

void AddTransactionForm::SubmitBtnClicked(){

    if(! inputValid()) {
        successMessage(false, "Empty fields");
        return;
    }

    Transaction t = Transaction();
    t.setDate(ui->transDateBox->date());
    t.setAmount(ui->transAmntBox->value());
    t.setType(ui->optIncome->isChecked()? "Income" : "Expenses");
    t.setCategory(ui->transCatBox->currentText());
    t.setDesc(ui->transDescBox->text());
    t.setAccount(ui->accountBox->currentText());
    t.setRecurring(ui->recurringCheckBox->isChecked());

    if(t.isRecurring()){
        t.setRecurringRange(ui->recurringRange->currentText());
        t.setRecurringValue(ui->recurringValue->value());
    }

    double tempSavings = get.getAccountValue("Savings");

    if(t.isFuture() || t.isRecurring()) set.scheduleTransaction(t);

    int result = set.addTransaction(t);

    switch(result){
    case -1:
        if(t.isRecurring()) successMessage(false, "Transaction scheduled, budget insufficient.");
        else successMessage(false, "Budget insufficient.");
        break;
    default:
        double removed = tempSavings - get.getAccountValue("Savings");
        if(removed!=0)           successMessage(true, Global::toCurrency(removed) + " removed from savings.");
        else if(t.isFuture())    successMessage(true, "Transaction scheduled.");
        else if(t.isRecurring()) successMessage(true, "Transaction added and scheduled.");
        else                     successMessage(true, "Transaction added successfully.");

        emit this->appRefresh();
        resetLabels();
        loadSchedule();
        break;
    }
}

void AddTransactionForm::resetLabels(){
    ui->transAmntBox->clear();
    ui->transCatBox->clear();
    ui->transDateBox->setDate(IPDate);
    ui->transDescBox->clear();
    ui->recurringCheckBox->setChecked(false);
    ui->recurringRange->setCurrentIndex(-1);
    ui->recurringValue->setValue(0);

    ui->optIncome->setAutoExclusive(false);
    ui->optExpenses->setAutoExclusive(false);
    ui->optIncome->setChecked(false);
    ui->optExpenses->setChecked(false);
    ui->optIncome->setAutoExclusive(true);
    ui->optExpenses->setAutoExclusive(true);
}

bool AddTransactionForm::inputValid() const {
    if (ui->transDescBox->text().isEmpty()
            ||ui->transCatBox->currentText().isEmpty()
            ||ui->accountBox->currentText().isEmpty()
            ||ui->transAmntBox->value() == 0
            || (ui->recurringCheckBox->isChecked()
                && (ui->recurringValue->value() == 0
                    || ui->recurringRange->currentText().isEmpty())))
         return false;
    else return true;
};

void AddTransactionForm::successMessage(bool success,QString text){
    if(success){
        ui->transReportLabels->setCurrentIndex(1);
        ui->transSuccessLabel->setText(text);
    }
    else {
        ui->transReportLabels->setCurrentIndex(2);
        ui->transErrorLabel->setText(text);
    }
}

void AddTransactionForm::loadSchedule(){
    while(!ui->scheduleLayout->isEmpty()){
        auto i = ui->scheduleLayout->takeAt(0);
        delete i->widget();
    }

    QList<Transaction> schedule = get.getSchedule();
    std::reverse(schedule.begin(), schedule.end());

    for( auto& r: schedule){
        QWidget* i = createWidgetSchedule(r);
        ui->scheduleLayout->insertWidget(0,i,Qt::AlignHCenter | Qt::AlignTop);
    }
}

QWidget* AddTransactionForm::createWidgetSchedule(Transaction& t){
    // main widget
    QWidget* i = new QWidget();
    i->resize(300, 100);
    i->setMinimumSize(QSize(300, 120));
    i->setMaximumSize(QSize(16777, 85));
    i->setStyleSheet("background-color: rgb(255, 255, 255); border-radius: 5px;"
                     "border: 0.5px solid rgb(222, 221, 218);");

    // horizontal box
    QHBoxLayout *horizontalLayout = new QHBoxLayout(i);
    horizontalLayout->setContentsMargins(15, 7, 15, 7);
    horizontalLayout->setSpacing(10);

    // icon
    QLabel* icon = new QLabel(i);
    icon->setFixedSize(QSize(30,30));
    if(t.getType() == "Income") icon->setPixmap(QPixmap(":/resources/income-schedule.png"));
    if(t.getType() == "Expenses") icon->setPixmap(QPixmap(":/resources/expense-schedule.png"));
    icon->setStyleSheet("border:0px;");
    icon->setScaledContents(true);
    horizontalLayout->addWidget(icon);

    // vertical box
    QVBoxLayout* verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(3);
    verticalLayout->setContentsMargins(-1, 5, -1, 10);
    horizontalLayout->addLayout(verticalLayout);

    // description
    QLabel* description = new QLabel(i);
    description->setMinimumSize(QSize(150, 25));
    description->setStyleSheet("font: 600 15px 'Montserrat'; color: rgb(0, 0, 0); border:0px;");
    verticalLayout->addWidget(description);
    description->setText(t.getDesc());

    // category
    QLabel* category = new QLabel(i);
    category->setStyleSheet("font: 12px 'Montserrat'; color: rgb(0, 0, 0); border:0px;");
    category->setText(t.getCategory());
    verticalLayout->addWidget(category);

    // recurrance
    QLabel* recurrance = new QLabel(i);
    recurrance->setStyleSheet("font: 12px 'Montserrat'; color: rgb(0, 0, 0); border:0px;");
    if(t.getRecurringValue()!=0)
        recurrance->setText(QString::number(t.getRecurringValue())+" "+t.getRecurringRange());
    else
        recurrance->setText("Not Recurring");
    verticalLayout->addWidget(recurrance);

    // next date
    QLabel* nextDate = new QLabel(i);
    nextDate->setStyleSheet("font: 12px 'Montserrat'; color: rgb(0, 0, 0); border:0px;");
    nextDate->setText("Next date: " + t.getDate().toString("dd/MM/yy"));
    verticalLayout->addWidget(nextDate);

    QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    // amount
    QLabel* amount = new QLabel(i);
    amount->setStyleSheet("font: 300 14px 'Montserrat'; color:black; border:0px;");
    horizontalLayout->addWidget(amount, 0, Qt::AlignRight);
    amount->setText(Global::toCurrency(t.getAmount()));


    // button 1
    QPushButton* cancelButton = new QPushButton(i);
    cancelButton->setFixedSize(QSize(30,30));
    cancelButton->setStyleSheet("QPushButton{background-color: white;"
                              "border-radius: 5px;"
                              "border:1px solid rgb(222, 221, 218)}"
                              "QPushButton:pressed {"
                              "background-color: rgb(222, 221, 218);}"
                              "QPushButton:checked{"
                              "background-color:rgb(222, 221, 218);}");

    cancelButton->setIcon(QPixmap(":/resources/cancel.png"));
    cancelButton->setIconSize(QSize(9,9));
    horizontalLayout->addWidget(cancelButton,0, Qt::AlignRight);

    connect(cancelButton, &QPushButton::released, this,
        [this, t](){
        QMessageBox msgBox = QMessageBox(this);
        msgBox.setStyleSheet("background-color:white;"
                             "font:14px 'Montserrat';");
        msgBox.setText("Are you sure you want to delete this schedule?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        switch (ret) {
          case QMessageBox::Yes:
              set.deleteSchedule(t);
              loadSchedule();
              break;
          case QMessageBox::Cancel:
              // do nothing
              break;
        }
                    });

    return i;
}
