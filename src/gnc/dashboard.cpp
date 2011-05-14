#include "dashboard.hpp"
#include "ui_dashboard.h"

Dashboard::Dashboard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dashboard)
{
    ui->setupUi(this);

    /* TODO: create appropriate methods */
    setCentralWidget(ui->firstPersonOverview);

    this->tabifyDockWidget(ui->txnEntryBasic, ui->txnEntrySplit);
    ui->txnEntryBasic->raise();
}

Dashboard::~Dashboard()
{
    delete ui;
}
