#include "startscreen.h"
#include "ui_startscreen.h"

StartScreen::StartScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartScreen)
{
    ui->setupUi(this);


    if(IPDate.year() == 0){
        ui->buttonsStackedWidget->setCurrentIndex(1);
    }
    else{
        ui->buttonsStackedWidget->setCurrentIndex(0);
    }

    ui->signBox->hide();
    ui->stackedWidget->setCurrentIndex(1);

    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startTrackerApp()));
    connect(ui->newAccountBtn, SIGNAL(clicked()), this, SLOT(newAccountBtn()));
    connect(ui->backButton, SIGNAL(clicked()), this, SLOT(backToAccounts()));
    connect(ui->newAccountIncome, SIGNAL(editingFinished()),this,SLOT(setSavingsLimit()));
    connect(ui->createAccountBtn, SIGNAL(clicked()), this, SLOT(createAccount()));
    connect(ui->currencyBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrency(int)));
    connect(ui->retryButton,&QPushButton::released,this,[](){
        // restart qApp
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    });
}

void StartScreen::setCurrency(int a){
    QString aa = ui->currencyBox->itemText(a).split(" ").at(0)+" ";
    ui->newAccountIncome->setPrefix(aa);
    ui->newAccountSavings->setPrefix(aa);
    ui->newAccountTotalSavings->setPrefix(aa);
}

StartScreen::~StartScreen()
{
    delete ui;
}

void StartScreen::startTrackerApp()
{
    ui->startButton->deleteLater();
    ui->signBox->show();
    ui->oldAccountBox->setMinimumHeight(400);
    displayAccounts();
    ui->currencyBox->addItems({"$ USD","€ Euro","£ GBP","¥ JPY","A$ AUD"});
}

void StartScreen::displayAccounts()
{
    accountsList = getAccounts();

    if(!accountsList.isEmpty()){
    for(QString &r: accountsList){

    // create button
        QPushButton *accountButton = new QPushButton();
        accountButton->setFixedSize(QSize(340, 66));
        r.remove(".db");
        accountButton->setText(" " + r);
        ui->userAccountsBox->insertWidget(0,accountButton);

        // connect button to main app
        connect(accountButton, &QPushButton::clicked,
            this, [=]() {

            // set up a connection on button click
            DbConnection a = DbConnection(r);

            // check beginning of month
            a.monthlyReset();

            // set user currency
            Global::setCurrency(a.getCurrency());

            // start application
            TrackerApp* AppWindow = new TrackerApp(this, r);
            AppWindow->show();
            close();}
        );

        // style button
        accountButton->setStyleSheet("QPushButton{ "
                               "background-color: rgb(231, 232, 240); "
                               "border-radius: 5px; "
                               "Text-align:left; "
                               "font: 500 11pt 'Montserrat'; "
                               "color:black; "
                               "padding: 10px; } "
                               "QPushButton:hover { "
                               "border:1px solid rgb(217, 219, 230);"
                               " border-style: inset; }"
                               " QPushButton:pressed { "
                               "background-color: rgb(217, 219, 230); }");
        accountButton->setIcon(QIcon(":/resources/usericon.png"));
        accountButton->setIconSize(QSize(50,50));
    }
    ui->noAccountsLabel->hide();
    }

    return;
}

void StartScreen::newAccountBtn()
{
    // hide old accounts and button, show new account widgets
    ui->stackedWidget->setCurrentIndex(0);
    ui->selectAccountLabel->setText("Enter account details");
    ui->newAccountErrorLabel->hide();
    ui->newAccountSavings->setMaximum(0);
    ui->currencyBox->setCurrentIndex(0);

}

void StartScreen::backToAccounts()
{
    // hide new account widgets, show old accounts & button
    ui->stackedWidget->setCurrentIndex(1);
    clearLabels();
    ui->selectAccountLabel->setText("Select your account");
}

void StartScreen::clearLabels()
{
    ui->newAccountName->clear();
    ui->newAccountIncome->clear();
    ui->newAccountSavings->clear();
    ui->newAccountTotalSavings->clear();
}

void StartScreen::setSavingsLimit(){
    ui->newAccountSavings->setMaximum(ui->newAccountIncome->value());
}

void StartScreen::createAccount()
{
    QString name = ui->newAccountName->text();
    double income = ui->newAccountIncome->value();
    double savings = ui->newAccountSavings->value();
    double totalSavings = ui->newAccountTotalSavings->value();
    QString currency = ui->currencyBox->currentText();

    // check if text field is empty
    if(ui->newAccountName->text().simplified() == ""){
        ui->newAccountErrorLabel->setText("No name entered");
        ui->newAccountErrorLabel->show();
    }
    // check if duplicate account name exists
    else if (checkDuplicate(name)){
        ui->newAccountErrorLabel->setText("Account name taken");
        ui->newAccountErrorLabel->show();
        return;
    }
    // create instance of tracker app
    else {
        clearLabels();
        DbConnection a = DbConnection(name);
        // set user currency
        Global::setCurrency(currency.split(" ").at(0) + " ");
        a.initAccount(income,savings,totalSavings,currency.split(" ").at(0) + " ");

        // Initialize Account and database connection


        TrackerApp* AppWindow = new TrackerApp(this, name);
        AppWindow->show();
        close();
    }
}

QStringList StartScreen::getAccounts(){

    // Create directory if not exists
    QDir accountsFolder(QStandardPaths::writableLocation
                        (QStandardPaths::DocumentsLocation)
                        +"/TrackerApp data/");
    if (!accountsFolder.exists()) {
        accountsFolder.mkpath(".");
        return {};
    }

    // Search databases in folder
    accountsFolder.setNameFilters(QStringList()<<"*.db");
    return accountsFolder.entryList();
}

bool StartScreen::checkDuplicate(QString a)
{
    for(auto &r: accountsList)
        if (a == r) return true;

    return false;
}
