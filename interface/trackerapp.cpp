#include "trackerapp.h"
#include "ui_trackerapp.h"

TrackerApp::TrackerApp(QWidget *parent, QString accountName)
    : QWidget(nullptr),
      ui(new Ui::TrackerApp),
      name(accountName)
{
    ui->setupUi(this);
    this->resize(parent->size());
    initApp();
    initSettings();
    refresh();
}

TrackerApp::~TrackerApp(){
    delete ui;
}

void TrackerApp::initApp(){

    // initialize widgets
    ui->stackedWidget->setCurrentIndex(0);
    ui->categoriesStackedWidget->setCurrentIndex(1);
    ui->transactionsDate->setDate(IPDate);
    ui->nameLabel->setText(name+"!");
    ui->fromBox->addItems({"Budget","Savings","Total Savings"});
    ui->toBox->addItems({"Budget","Savings","Total Savings"});
    ui->transferAmountBox->setPrefix(Global::CurrencySymbol()+" ");
    ui->transferAmountBox->setMaximum(0);

    // Add Transaction class widget
    QWidget *AddTransactionWidget = new AddTransactionForm(this, name);
    ui->leftLayout->insertWidget(0,AddTransactionWidget);
    connect(AddTransactionWidget, SIGNAL(appRefresh()), this, SLOT(refresh()));

    connectBtns();

}

void TrackerApp::connectBtns(){
    connect(ui->transactionsDate, SIGNAL(userDateChanged(QDate)),this, SLOT(loadTransactions()));
    connect(ui->transferButton,SIGNAL(released()), this, SLOT(transferAmount()));

    connect(ui->previousDayBtn, &QPushButton::released, this,
        [this](){ ui->transactionsDate->setDate(ui->transactionsDate->date().addDays(-1));});

    connect(ui->nextDayBtn, &QPushButton::released, this,
        [this](){ ui->transactionsDate->setDate(ui->transactionsDate->date().addDays(1));});

    connect(ui->todayBtn, &QPushButton::released, this,
        [this](){ ui->transactionsDate->setDate(IPDate); });

    connect(ui->expensesButton, &QPushButton::released, this,
        [this]{ ui->categoriesStackedWidget->setCurrentIndex(0);
                ui->incomeButton->setChecked(false);
                ui->budgetButton->setChecked(false);
                ui->expensesButton->setChecked(true);});

    connect(ui->incomeButton, &QPushButton::released, this,
        [this]{ ui->categoriesStackedWidget->setCurrentIndex(2);
                ui->expensesButton->setChecked(false);
                ui->budgetButton->setChecked(false);
                ui->incomeButton->setChecked(true);});

    connect(ui->budgetButton, &QPushButton::released, this,
        [this]{ ui->categoriesStackedWidget->setCurrentIndex(1);
                ui->expensesButton->setChecked(false);
                ui->incomeButton->setChecked(false);
                ui->budgetButton->setChecked(true);});

    connect(ui->fromBox,&QComboBox::currentTextChanged,this,[this](QString a){
        ui->transferAmountBox->setMaximum(get.getAccountValue(a));
    });

    connect(ui->settingsButton, &QPushButton::released,this,[this](){
        ui->stackedWidget->setCurrentIndex(1);
    });

    connect(ui->backSettingsButton, &QPushButton::released,this,[this](){
        ui->stackedWidget->setCurrentIndex(0);
        initSettings();
        refresh();
    });

    connect(ui->githubButton,&QPushButton::released,[](){
        QDesktopServices::openUrl(QUrl("https://github.com/Mario-az/TrackerApp", QUrl::TolerantMode));
    });

}

void TrackerApp::refresh(){

    double value;

    // Set cards values

    value = get.getAccountValue("Budget");
    ui->remainingBudget->setText(Global::toCurrency(value));

    value = get.getAccountValue("Income");
    ui->monthlyIncome->setText(Global::toCurrency(value));

    value = get.getAccountValue("Expenses");
    ui->monthlyExpenses->setText(Global::toCurrency(value));

    value = get.getAccountValue("Savings");
    ui->monthlySavings->setText(Global::toCurrency(value));

    value = get.getAccountValue("Total Savings");
    ui->totalSavings->setText(Global::toCurrency(value));

    loadExpensesCategories();

    loadIncomesCategories();

    loadTransactions();

}

void TrackerApp::transferAmount(){
    QString from = ui->fromBox->currentText();
    QString to = ui->toBox->currentText();
    double amount = ui->transferAmountBox->value();

    if(from == to || from.isEmpty() || to.isEmpty() || amount == 0)
    {
        ui->transferButton->setIcon(QPixmap(":/images/remove.png"));
        return;
    }
    else if(set.transferAmount(from, to, amount)){
        ui->transferButton->setIcon(QPixmap(":/images/success.png"));
        ui->transferAmountBox->setValue(0);
        refresh();
        return;
    }
    else{
        ui->transferButton->setIcon(QPixmap(":/images/remove.png"));
        return;
    }
}

void TrackerApp::loadIncomesCategories(){

    QVector<QPair<QString,double>> values {};

    // get incomes from date selected
    for(auto &r:get.getIncomesCategories()){
        QPair<QString,double> a;
        a.first = r;
        a.second = get.getIncomesValue(r,IPDate);
        values.push_back(a);
    }

    // clear transactions parent widget
    while(!ui->incCategoriesSummary->isEmpty()){
        auto i = ui->incCategoriesSummary->takeAt(0);
        delete i->widget();
    }

    // sort in descending order by value
    std::sort(values.begin(), values.end(), []
              (const std::pair<QString,double> &v1, const std::pair<QString,double> v2)
                {
                    return v1.second > v2.second;
                }
    );

    // set the intial bar stylesheet
    QString barStyleSheet {"::chunk{border-radius:3px;"
                           "background-color: qlineargradient(x0: 0, x2: 1, "};
    double startRatio {0};
    double endRatio {};

    // colors of the bar
    QVector<QString> colors ={"#2a59ff","#e136d3","#ff4297","#ff8365","#ffc24f","#f9f871","#B1A24A","#98C807","#ABB8AF"};
    int i = 0;
    bool a = true;

    int x{0},y{0},itemsPerRow{9};
    for (auto &r: values){

        endRatio = startRatio + r.second/get.getAccountValue("Income");

        // add style to bar using gradient
        if(endRatio<=1.001 && r.second !=0 && a){
            if(endRatio>1) endRatio = 1;
            barStyleSheet.append(" stop: " + QString::number(startRatio) + " " + colors.at(i) + ","
                                 " stop: " + QString::number(  endRatio) + " " + colors.at(i) + ", ");
            startRatio = endRatio + 0.0001;

            if(endRatio==1) a = false;
        }

        // add the color, name and value of each category
        QWidget* category = new QWidget();
        category->setFixedSize(100, 80);
        QLabel* dot = new QLabel();
        QLabel* type = new QLabel();
        QLabel* amount = new QLabel();
        QVBoxLayout *layout = new QVBoxLayout;
        category->setLayout(layout);
        dot->setText("●");
        dot->setStyleSheet("font: 25px;"
                           "color:"+colors.at(i)+";");
        layout->addWidget(dot, 0, Qt::AlignHCenter);
        type->setText(r.first);
        type->setStyleSheet("font: 500 10pt 'Montserrat';"
                            "border:0px solid gray;");
        layout->addWidget(type, 0, Qt::AlignHCenter);
        amount->setText(Global::toCurrency(r.second));
        amount->setStyleSheet("font: 300 9pt 'Montserrat';"
                              "border:0px solid gray;");
        layout->addWidget(amount, 0, Qt::AlignHCenter);

        ui->incCategoriesSummary->addWidget(category, x, y, Qt::AlignHCenter);
        ++i;
        y++;
        if (y%itemsPerRow == 0) {x++; y=0;}
        }
    barStyleSheet.chop(2);
    barStyleSheet.append(")}");

    ui->progressBar2->setStyleSheet(barStyleSheet);
    if(get.getAccountValue("Income") == 0)
        ui->progressBar2->setValue(0);
    else ui->progressBar2->setValue(100);

}

void TrackerApp::loadExpensesCategories(){

    // clear already present widgets
    while(!ui->expCategoriesSummary->isEmpty()){
        auto i = ui->expCategoriesSummary->takeAt(0);
        delete i->widget();
    }

    // initialize values vector
    QVector<QPair<QString,double>> values;

    // add values to vector
    for(auto &r:get.getExpensesCategories()){
        QPair<QString,double> a;
        a.first = r;
        a.second = get.getExpensesValue(r,IPDate);
        values.append(a);
    }

    // sort in descending order by value
    std::sort(values.begin(), values.end(), []
              (const std::pair<QString,double> &v1, const std::pair<QString,double> v2)
                {
                    return v1.second > v2.second;
                }
    );

    // add initial bar stylesheet
    QString barStyleSheet {"::chunk{border-radius:3px;"
                           "background-color: qlineargradient(x0: 0, x2: 1, "};
    QVector<QString> colors ={"#4260FF","#FF4264","#FF9D42","#FFE142","#42FF77","#42C6FF","#CE42FF","#FF42B3","#98C807"};
    int i = 0;
    bool a = true;

    double startRatio {0};
    double endRatio {0};

    for (auto &r: values){

        endRatio = startRatio + r.second/get.getAccountValue("Expenses");

        // add style to bar using gradient
        if(endRatio<=1.001 && r.second !=0 && a){
            if(endRatio>1) endRatio = 1;
            barStyleSheet.append(" stop: " + QString::number(startRatio) + " " + colors.at(i) + ","
                                 " stop: " + QString::number(  endRatio) + " " + colors.at(i) + ", ");
            startRatio = endRatio + 0.0001;

            if(endRatio==1) a = false;
        }

        // add the color, name and value of each category
        QWidget* category = new QWidget();
        category->setFixedSize(100, 80);
        QLabel* dot = new QLabel();
        QLabel* type = new QLabel();
        QLabel* amount = new QLabel();
        QVBoxLayout *layout = new QVBoxLayout;
        category->setLayout(layout);
        dot->setText("●");
        dot->setStyleSheet("font: 25px;"
                           "color:"+colors.at(i)+";");
        layout->addWidget(dot, 0, Qt::AlignHCenter);
        type->setText(r.first);
        type->setStyleSheet("font: 500 10pt 'Montserrat';"
                            "border:0px solid gray;");
        layout->addWidget(type, 0, Qt::AlignHCenter);
        amount->setText(Global::toCurrency(r.second));
        amount->setStyleSheet("font: 300 9pt 'Montserrat';"
                              "border:0px solid gray;");
        layout->addWidget(amount, 0, Qt::AlignHCenter);

        ui->expCategoriesSummary->addWidget(category, Qt::AlignHCenter);
        ++i;
        }

    barStyleSheet.chop(2);
    barStyleSheet.append(")}");

    ui->progressBar->setStyleSheet(barStyleSheet);
    ui->progressBar->setValue((int)round(get.getAccountValue("Expenses")/
                                         (get.getAccountValue("Budget")+get.getAccountValue("Expenses"))
                                         *100));
}

void TrackerApp::loadTransactions()
{
    // clear transactions list
    while(!ui->transactionsList->isEmpty()){
        auto i = ui->transactionsList->takeAt(0);
        delete i->widget();
    }

    // load scheduled and added transactions
    QDate date = ui->transactionsDate->date();
    QList<Transaction> schedule = get.getSchedule(date);
    QList<Transaction> data = get.getTransactions(date);
    std::reverse(data.begin(), data.end());

    // return of no transactions
    if(data.isEmpty()){
        ui->transactionsStackedWidget->setCurrentIndex(1);
        return; }

    // else continue loading transactions
    ui->transactionsStackedWidget->setCurrentIndex(0);

    int x{0}, y{0};

    for( auto& r: schedule){
        QWidget* i = createWidgetSchedule(r);
        ui->transactionsList->addWidget(i,x,y,Qt::AlignTop);
        if(++y==2) {y=0; x++;}
    }

    for( auto& r: data){
        QWidget* i = createWidgetTransaction(r);
        ui->transactionsList->addWidget(i,x,y,Qt::AlignTop);
        if(++y==2) {y=0; x++;}
    }
}

QWidget* TrackerApp::createWidgetTransaction(Transaction& t){
    // main widget
    QWidget* i = new QWidget();
    i->setObjectName("main");
    i->resize(300, 75);
    i->setMinimumSize(QSize(300, 75));
    i->setMaximumSize(QSize(16777, 75));
    i->setStyleSheet("background-color: rgb(255, 255, 255); border-radius: 5px;"
                     "border: 0.5px solid rgb(222, 221, 218);");

    // horizontal box
    QHBoxLayout *horizontalLayout = new QHBoxLayout(i);
    horizontalLayout->setContentsMargins(15, 10, 15, 10);
    horizontalLayout->setSpacing(15);

    // icon
    QLabel* icon = new QLabel(i);
    icon->setFixedSize(QSize(30,30));
    if(t.getType() == "Income") icon->setPixmap(QPixmap(":/images/income.png"));
    if(t.getType() == "Expenses") icon->setPixmap(QPixmap(":/images/expense.png"));
    icon->setScaledContents(true);
    icon->setStyleSheet("border:0px;");
    horizontalLayout->addWidget(icon);

    // vertical box
    QVBoxLayout* verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(8);
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

    QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    // amount
    QLabel* amount = new QLabel(i);
    amount->setStyleSheet("font: 300 14px 'Montserrat'; color:black; border:0px;");
    horizontalLayout->addWidget(amount, 0, Qt::AlignRight);
    amount->setText(Global::toCurrency(t.getAmount()));


    // button
    QPushButton* editButton = new QPushButton(i);
    editButton->setFixedSize(QSize(30,30));
    editButton->setStyleSheet("QPushButton{background-color: white;"
                              "border-radius: 5px;"
                              "border:1px solid rgb(222, 221, 218);"
                              "color: black;}"
                              "QPushButton:pressed {"
                              "background-color: rgb(222, 221, 218);}"
                              "QPushButton:checked{"
                              "background-color:rgb(222, 221, 218);}");

    editButton->setIcon(QPixmap(":/images/editPen.png"));
    editButton->setIconSize(QSize(12,12));
    horizontalLayout->addWidget(editButton,0, Qt::AlignRight);

    connect(editButton, &QPushButton::released, this,
        [this, t](){

            QDialog* editor = new editTransactionDialog(this,t.getID());
            connect(editor,SIGNAL(transactionUpdate()),this,SLOT(refresh()));
            editor->exec();
                    });

    return i;
}

QWidget* TrackerApp::createWidgetSchedule(Transaction& t){
    // main widget
    QWidget* i = new QWidget();
    i->resize(300, 75);
    i->setMinimumSize(QSize(300, 75));
    i->setMaximumSize(QSize(16777, 75));
    i->setStyleSheet("background-color: rgb(255, 255, 255); border-radius: 5px;"
                     "border: 0.5px solid rgb(222, 221, 218);");

    // horizontal box
    QHBoxLayout *horizontalLayout = new QHBoxLayout(i);
    horizontalLayout->setContentsMargins(15, 10, 15, 10);
    horizontalLayout->setSpacing(15);

    // icon
    QLabel* icon = new QLabel(i);
    icon->setFixedSize(QSize(30,30));
    if(t.getType() == "Income") icon->setPixmap(QPixmap(":/images/income-schedule.png"));
    if(t.getType() == "Expenses") icon->setPixmap(QPixmap(":/images/expense-schedule.png"));
    icon->setScaledContents(true);
    icon->setStyleSheet("border:0px;");
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

    cancelButton->setIcon(QPixmap(":/images/cancel.png"));
    cancelButton->setIconSize(QSize(9,9));
    horizontalLayout->addWidget(cancelButton,0, Qt::AlignRight);

    connect(cancelButton, &QPushButton::released, this,
        [this, i, t](){
        i->hide();
        set.updateSchedule(t);
                    });

    // button 2
    QPushButton* acceptButton = new QPushButton(i);
    acceptButton->setFixedSize(QSize(30,30));
    acceptButton->setStyleSheet("QPushButton{background-color: white;"
                              "border-radius: 5px;"
                              "border:1px solid rgb(222, 221, 218);}"
                              "QPushButton:pressed {"
                              "background-color: rgb(222, 221, 218);}"
                              "QPushButton:checked{"
                              "background-color:rgb(222, 221, 218);}");

    acceptButton->setIcon(QPixmap(":/images/accept.png"));
    acceptButton->setIconSize(QSize(12,12));
    horizontalLayout->addWidget(acceptButton,0, Qt::AlignRight);

    connect(acceptButton, &QPushButton::released, this,
        [this, i, t](){

        int result = set.addTransaction(t);
        switch(result){
        case -1:
            i->setStyleSheet("background-color: rgb(255, 255, 255); border-radius: 5px;"
                             "border: 0.5px solid #FF4264;");
            break;
        default:
            set.updateSchedule(t);
            refresh();

            break;

            }});

    return i;
}

void TrackerApp::initSettings(){
    ui->settingsStackedWidget->setCurrentIndex(0);
    ui->settingsNameLabel->setText(name);
    ui->settingsIncomeSpinBox->setValue(get.getAccountValue("Default Income"));
    ui->settingsIncomeSpinBox->setPrefix(Global::CurrencySymbol()+" ");
    ui->settingsSavingsSpinBox->setValue(get.getAccountValue("Default Savings"));
    ui->settingsSavingsSpinBox->setPrefix(Global::CurrencySymbol()+" ");
    ui->settingsTotalSavingsSpinBox->setValue(get.getAccountValue("Total Savings"));
    ui->settingsTotalSavingsSpinBox->setPrefix(Global::CurrencySymbol()+" ");

    connect(ui->updateAccountButton,&QPushButton::released, this,[this](){

        // update account
        set.updateAccount(ui->settingsIncomeSpinBox->value(),
                          ui->settingsSavingsSpinBox->value(),
                          ui->settingsTotalSavingsSpinBox->value());

        successMessage(true,"Account updated.");

        // if name is different and not empty
        if(ui->settingsNameLabel->text().simplified() != name
                && ui->settingsNameLabel->text().simplified() != ""){

            // check duplicate account name function
            auto checkDuplicate = [](QStringList accounts, QString name){
                name = name+".db";
                for(auto &r: accounts)
                    if (name == r) return true;
                return false;
            };

            QDir accountsFolder(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                                +"/TrackerApp data/");
            accountsFolder.setNameFilters(QStringList()<<"*.db");
            QStringList accounts = accountsFolder.entryList();

            if(! checkDuplicate(accounts,ui->settingsNameLabel->text().simplified())){
                // old file location
                QString oldName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
                oldName.append("/TrackerApp data/");
                oldName.append(name);
                oldName.append(".db");

                // new file location
                QString newName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
                newName.append( "/TrackerApp data/");
                newName.append(ui->settingsNameLabel->text().simplified());
                newName.append(".db");

                // rename file
                QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
                QFile::rename(oldName,newName);
                name = ui->settingsNameLabel->text().simplified();

                // restart qApp
                qApp->quit();
                QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
            }
            else{
                successMessage(false, "Account name invalid.");
            }


        }

    });

    connect(ui->deleteAccountButton,&QPushButton::released, this,[this](){
        QMessageBox msgBox = QMessageBox(this);
        msgBox.setStyleSheet("background-color:white;"
                             "font:14px 'Montserrat';");
        msgBox.setText("Are you sure you want to delete your account?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        QString account = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        account.append("/TrackerApp data/");
        account.append(name);
        account.append(".db");
        QFile file (account);

        switch (ret) {
          case QMessageBox::Yes:
                QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
                file.remove();

              // restart qApp
              qApp->quit();
              QProcess::startDetached(qApp->arguments()[0], qApp->arguments());

              break;
          case QMessageBox::Cancel:
              // do nothing
              break;
        }
    });

    connect(ui->exportButton,&QPushButton::released, this,[this](){
        QMessageBox msgBox = QMessageBox(this);
        msgBox.setStyleSheet("background-color:white;"
                             "font:14px 'Montserrat';");
        msgBox.setText("Feature under development.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        QString account = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        account.append("/TrackerApp data/");
        account.append(name);
        account.append(".db");

        switch (ret) {
          case QMessageBox::Ok:
              break;
        }
    });
}

void TrackerApp::successMessage(bool a, QString message){

    if(a) ui->settingsStackedWidget->setCurrentIndex(1);
    else  ui->settingsStackedWidget->setCurrentIndex(2);

    ui->SuccessLabel->setText(message);
    ui->ErrorLabel->setText(message);
}
