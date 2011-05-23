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
    ui->cmboTransferFrom->setModel(this->m_accountTreeModel);
    ui->cmboTransferTo->setModel(this->m_accountTreeModel);
    qDebug() <<"Executed load tree in cmbo";
}

} // END namespace gnc
