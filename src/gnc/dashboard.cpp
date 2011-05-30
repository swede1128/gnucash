/*
 * Copyright (C) 2011 Free Software Foundation, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * @file
 * Dashboard for embedding various dock widgets.
 */


#include "dashboard.hpp"
#include "ui_dashboard.h"

#include <QtGui>
#include <QAbstractItemModel>

namespace gnc
{

Dashboard::Dashboard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dashboard)
{
    ui->setupUi(this);

    /* Initialise */
    index = 0;
    //$$$$$$$$accountName = accountName->gnc_book_get_root_account(get());

    /* Generate UI */
    setUiWidgets();
    setBasicTxnEntryFormLayout();
    setCentralWidget(ui->firstPersonOverview);
    this->tabifyDockWidget(ui->dockwBasicTxn, ui->dockwSplitTxn);
    ui->dockwBasicTxn->raise();

    connect(btnCreateTxn, SIGNAL(clicked()),
            this, SLOT(on_btnCreateTxn_clicked()));
}

Dashboard::~Dashboard()
{
    delete ui;
}

/** Initialise widgets to startup defaults */
void
Dashboard::setUiWidgets()
{
    lblDescription  = new QLabel(tr("Description:"));
    lblDate         = new QLabel(tr("Date:"));
    lblTransferFrom = new QLabel(tr("Transfer From:"));
    lblTransferTo   = new QLabel(tr("TransferTo:"));
    lblAmount       = new QLabel(tr("Amount:"));
    lblMemo         = new QLabel(tr("Memo:"));
    lblNum          = new QLabel(tr("Num:"));
    comboTransferFrom = new QComboBox();
    comboTransferTo   = new QComboBox();
    lineDescription = new QLineEdit();
    lineAmount      = new QLineEdit();
    lineMemo        = new QLineEdit();
    lineNum         = new QLineEdit();
    dateTxnDate = new QDateEdit();
    btnCreateTxn = new QPushButton(tr("Create Transaction"));
}

/** Layout for data entry. Type: Form Based
 *
 * Set this as default to make this layout as default
 * for all data entry widgets. */
void
Dashboard::setBasicTxnEntryFormLayout()
{
    gridBasicTxnEntry = new QGridLayout(ui->dockcBasicTxn);

    gridBasicTxnEntry->addWidget(lblDescription, 0, 0);
    gridBasicTxnEntry->addWidget(lineDescription, 0, 1);

    gridBasicTxnEntry->addWidget(lblDate, 1, 0);
    gridBasicTxnEntry->addWidget(dateTxnDate, 1, 1);

    gridBasicTxnEntry->addWidget(lblTransferFrom, 2, 0);
    gridBasicTxnEntry->addWidget(comboTransferFrom, 2, 1);

    gridBasicTxnEntry->addWidget(lblTransferTo, 3, 0);
    gridBasicTxnEntry->addWidget(comboTransferTo, 3, 1);

    gridBasicTxnEntry->addWidget(lblAmount, 4, 0);
    gridBasicTxnEntry->addWidget(lineAmount, 4, 1);

    gridBasicTxnEntry->addWidget(lblMemo, 5, 0);
    gridBasicTxnEntry->addWidget(lineMemo, 5, 1);

    gridBasicTxnEntry->addWidget(lblNum, 6, 0);
    gridBasicTxnEntry->addWidget(lineNum, 6, 1);

    gridBasicTxnEntry->addWidget(btnCreateTxn, 7, 1);
}

void
Dashboard::loadAccountsTreeComboBox()
{
    comboTransferFrom->setModel(m_accountListModel);
    comboTransferTo->setModel(m_accountListModel);
}

/***** Slots *****/

/** Create Transaction button for Basic Transaction Entry dock widget */
void
Dashboard::on_btnCreateTxn_clicked()
{
    /* Allocate memory for a new transaction */
    index = comboTransferFrom->currentIndex();
    account = m_accountListModel->at(index);
    book = gnc_account_get_book(account);
    transaction = ::xaccMallocTransaction(book);

    /*
    ::xaccTransBeginEdit(transaction);

    ::xaccTransSetDatePostedSecs(transaction, 1234567);
    ::xaccTransSetNum(transaction, "X23");

    currency = ::xaccAccountGetCommodity(account);
    ::xaccTransSetCurrency(transaction, currency);

    split = xaccMallocSplit(book);
    xaccTransAppendSplit(transaction, split);
    xaccAccountInsertSplit(account, split);
    amount = gnc_numeric_create(123, 100);
    xaccSplitSetValue(split, amount);
    xaccSplitSetAmount(split, amount);
    */
}

} // END namespace gnc
