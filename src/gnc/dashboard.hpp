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

#ifndef DASHBOARD_HPP
#define DASHBOARD_HPP

#include "config.h"

extern "C"
{
#include "qof.h"
#include "engine/Account.h"
#include "engine/Transaction.h"
//#include "engine/gnc-hooks.h"
}

#include "AccountItemModel.hpp"

#include <QMainWindow>
#include <QAbstractItemModel>
#include <QtGui>

namespace Ui {
    class Dashboard;
}

namespace gnc
{

class Dashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = 0);
    ~Dashboard();

    AccountListModel *m_accountListModel;
    void loadAccountsTreeComboBox();

    //QDate getDatePosted() const { return dateTxnDate; }

private:
    Ui::Dashboard *ui;

    /* UI widgets */
    QGridLayout *gridBasicTxnEntry;
    QHBoxLayout *hbox;
    QVBoxLayout *vbox;
    QLabel *lblDescription;
    QLabel *lblDate;
    QLabel *lblTransferFrom;
    QLabel *lblTransferTo;
    QLabel *lblAmount;
    QLabel *lblMemo;
    QLabel *lblNum;
    QComboBox *comboTransferFrom;
    QComboBox *comboTransferTo;
    QLineEdit *lineDescription;
    QLineEdit *lineAmount;
    QLineEdit *lineMemo;
    QLineEdit *lineNum;
    QDateEdit *dateTxnDate;
    QPushButton *btnCreateBasicTxn;

    /* Transaction related data types */    
    ::QofBook *book;
    ::Transaction *transaction;
    ::gnc_commodity *currency;

    /* For conversion of QString to char */
    //QByteArray baNum;
    //const char *constNum;

    int index;
    ::Account *account;
    ::Split *split;
    ::gnc_numeric amount;

    int index2;
    ::Account *account2;
    ::Split *split2;
    int intAmount2;
    ::gnc_numeric amount2;

    void setUiWidgets();
    void setBasicTxnEntryFormLayout();

private slots:
    void on_btnCreateBasicTxn_clicked();
};

} // END namespace gnc

#endif // DASHBOARD_HPP
