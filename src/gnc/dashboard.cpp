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
#include<QDockWidget>
#include<QComboBox>
#include "dashboard.hpp"
#include "ui_dashboard.h"
#include "TreeCombo.hpp"

#include<QTreeView>
#include<QEvent>
namespace gnc
{

Dashboard::Dashboard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dashboard)
{
    ui->setupUi(this);

    setCentralWidget(ui->firstPersonOverview);

    this->tabifyDockWidget(ui->txnEntryBasic, ui->txnEntrySplit);
    ui->txnEntryBasic->raise();
}

Dashboard::~Dashboard()
{
    delete ui;
}

void
Dashboard::loadAccountsTreeComboBox()
{
    TreeComboBox * cmboTransferFrom = new TreeComboBox(ui->dockwBasicTxn);
    cmboTransferFrom->move(10,80);
    cmboTransferFrom->show();
    cmboTransferFrom->setModel(this->m_accountTreeModel);
    cmboTransferFrom->resize(190,25);

    TreeComboBox * cmboTransferTo = new TreeComboBox(ui->dockwBasicTxn);
    cmboTransferTo->move(210,80);
    cmboTransferTo->show();
    cmboTransferTo->setModel(this->m_accountTreeModel);
    cmboTransferTo->resize(190,25);
    cmboTransferFrom->lineEdit();

    qDebug() <<"Executed load tree in cmbo";
}

} // END namespace gnc
