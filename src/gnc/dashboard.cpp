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
 *
 * Dock Widgets:
 * 1) Transfer Funds
 * 2) First Person Overview
 */

#include "dashboard.hpp"
#include "ui_dashboard.h"

#include <QtGui>
#include <QAbstractItemModel>
#include <QDate>

namespace gnc
{
Dashboard::Dashboard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dashboard)
{
    ui->setupUi(this);

    // Generate UI
    setUiWidgets();
    setBasicTxnEntryFormLayout();
    setCentralWidget(ui->firstPersonOverview);
    //this->tabifyDockWidget(ui->dockwBasicTxn, ui->dockwSplitTxn);
    //ui->dockwBasicTxn->raise();
    ui->dockwSplitTxn->hide();

    connect(btnCreateBasicTxn, SIGNAL(clicked()),
            this, SLOT(on_btnCreateBasicTxn_clicked()));
}

Dashboard::~Dashboard()
{
    delete ui;
}

/** Initialise widgets to startup defaults */
void
Dashboard::setUiWidgets()
{
    numer = 0.0;

    lblDescription  = new QLabel(tr("Description:"));
    lblDate         = new QLabel(tr("Date:"));
    lblTransferFrom = new QLabel(tr("Transfer From:"));
    lblTransferTo   = new QLabel(tr("TransferTo:"));
    lblAmount       = new QLabel(tr("Amount:"));
    lblMemo         = new QLabel(tr("Memo:"));
    lblNum          = new QLabel(tr("Num:"));
    comboTransferFrom = new QComboBox();
    comboTransferFrom->addItem(tr("- NA -"));
    comboTransferTo   = new QComboBox();
    comboTransferTo->addItem(tr("- NA -"));
    lineDescription = new QLineEdit();
    lineAmount      = new QLineEdit();
    lineAmount->setText(tr("0.00"));
    lineAmount->setValidator(new QDoubleValidator(0.0, 100000000000.0, 2, this));
    lineMemo        = new QLineEdit();
    lineNum         = new QLineEdit();
    dateTxnDate = new QDateEdit();
    dateVal = QDate::currentDate();
    dateTxnDate->setDate(dateVal);
    btnCreateBasicTxn = new QPushButton(tr("Create Transaction"));
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

    gridBasicTxnEntry->addWidget(btnCreateBasicTxn, 7, 1);
}

void
Dashboard::loadAccountsTreeComboBox()
{
    comboTransferFrom->setModel(m_accountListModel);
    comboTransferTo->setModel(m_accountListModel);
}

void
Dashboard::clearFields()
{
    lineDescription->clear();
    lineAmount->clear();
    lineMemo->clear();
    lineNum->clear();
}

/***** Slots *****/

/** "Create Transaction" button for Basic Transaction Entry dock widget */
void
Dashboard::on_btnCreateBasicTxn_clicked()
{
    if (lineDescription->text().isEmpty())
    {
        QMessageBox::StandardButton warnEmptyDesc;
        warnEmptyDesc = QMessageBox::warning(this, tr("Empty Description"), "You have not set a description. Are you sure you want to create a transaction with no description?", QMessageBox::Yes | QMessageBox::No);
        if (warnEmptyDesc == QMessageBox::No)
            return;
    }

    // Allocate memory and start editing a new transaction
    int index = comboTransferFrom->currentIndex();
    account = m_accountListModel->at(index);
    book = gnc_account_get_book(account);
    transaction = ::xaccMallocTransaction(book);
    ::xaccTransBeginEdit(transaction);

    // Populate transaction details
    dateVal = dateTxnDate->date();
    ::xaccTransSetDate(transaction, dateVal.day(), dateVal.month(),
                       dateVal.year());

    ::xaccTransSetNum(transaction, lineNum->text().toUtf8());
    ::xaccTransSetDescription(transaction, lineDescription->text().toUtf8());

    currency = xaccAccountGetCommodity(account);
    ::xaccTransSetCurrency(transaction, currency);

    denom = ::gnc_commodity_get_fraction(currency);

    // Populate split 1
    // Check whether the account for this split is a placeholder
    qDebug() << ::xaccAccountGetPlaceholder(account);
    // ^^ does not work as expected, returns false for placeholder accounts too. Why?

    split = xaccMallocSplit(book);
    ::xaccTransAppendSplit(transaction, split);
    ::xaccAccountInsertSplit(account, split);

    numer = lineAmount->text().toDouble();
    amount = ::double_to_gnc_numeric(numer, denom,
                                     GNC_HOW_DENOM_REDUCE | GNC_HOW_RND_NEVER);
    ::xaccSplitSetValue(split, amount);
    ::xaccSplitSetAmount(split, amount);

    // Populate split 2
    split2 = ::xaccMallocSplit(book);
    ::xaccTransAppendSplit(transaction, split2);
    int index2 = comboTransferTo->currentIndex();
    account2 = m_accountListModel->at(index2);
    ::xaccAccountInsertSplit(account2, split2);

    amount2 = ::gnc_numeric_neg(amount);
    ::xaccSplitSetValue(split2, amount2);
    ::xaccSplitSetAmount(split2, amount2);

    // Finally commit the transaction to storage backend.
    ::xaccTransCommitEdit(transaction);

    statusBar()->showMessage(tr("Transaction has been created"));
    clearFields();
}

} // END namespace gnc
