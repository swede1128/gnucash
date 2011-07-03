#include "ViewletView.hpp"

namespace gnc
{

ViewletView::ViewletView(QWidget *parent) :
    QWidget(parent)
{
    setViewlet(parent);

    connect(btnSelectAccount, SIGNAL(clicked()),
            this, SLOT(on_btnSelectAccount_clicked()));
}

void
ViewletView::setViewlet(QWidget *parent)
{
    /* Initialize widgets to defaults */
    comboAccountsList = new QComboBox();
    comboAccountsList->addItem(tr("-NA-"));
    btnSelectAccount = new QPushButton(tr("Select Account"));

    /* Container layout for one viewlet */
    gridFPO = new QGridLayout(parent);

    /* First Row of the viewlet; selection widgets */
    groupSelection = new QWidget();
    vSelectionLayout = new QVBoxLayout();
    groupSelection->setLayout(vSelectionLayout);
    vSelectionLayout->addWidget(comboAccountsList);
    vSelectionLayout->addWidget(btnSelectAccount);

    /* Second row of the viewlet; display accounts data */
    colMiniJournalFirst = new QGroupBox();
    vColLayout = new QVBoxLayout();
    colMiniJournalFirst->setLayout(vColLayout);

    gridFPO->addWidget(groupSelection, 0, 0);
    gridFPO->addWidget(colMiniJournalFirst, 1, 0);
}

void
ViewletView::loadAccountsTreeComboBox(AccountListModel * const m_accountsListModel)
{
    accountsList = m_accountsListModel;
    comboAccountsList->setModel(accountsList);
}

/** GFunc called for gSplitList processing */
void
ViewletView::splits_func(gpointer data, gpointer user_data)
{
    ::Split const * pSplit;
    pSplit = (::Split *) data;
    //
    qDebug()<<"~~~~~~~~~~";

    // return pointer of parent account;
    qDebug()<<"~ Parent * \t"<<::xaccSplitGetAccount(pSplit);

    // return action string
    qDebug()<<"~ Action \t"<<::xaccSplitGetAction(pSplit);

    // return amount
    //qDebug()<<"~ Amount \t"<<::xaccSplitGetAmount();

    // balance including this split @see xaccSplitGetClearedBalance
    //qDebug()<<"~ Balance \t"<<::xaccSplitGetBalance();

    // account code
    qDebug()<<"~  Code \t"<<::xaccSplitGetCorrAccountCode(pSplit);

    // account name
    qDebug()<<"~  Account \t"<<::xaccSplitGetCorrAccountName(pSplit);

    //
    qDebug()<<"~ Reconcile \t"<<::xaccSplitGetReconcile(pSplit);

    //Get pointer to parent txn
    qDebug()<<"~ Parent Txn **"<<xaccSplitGetParent(pSplit);
    ::Transaction *parentTxn = xaccSplitGetParent(pSplit);

    qDebug()<<"~ Description \t"<<::xaccTransGetDescription(parentTxn);

    qDebug()<<"~ Notes \t"<<::xaccTransGetNotes(parentTxn);

    qDebug()<<"~ Num \t"<<::xaccTransGetNum(parentTxn);

    qDebug()<<::xaccSplitGetMemo(pSplit);

    // xaccSplitGetOtherSplit to quickly get the other * split in a two split txn
}

/***** Slots *****/

void
ViewletView::on_btnSelectAccount_clicked()
{
    selectedAccountIndex = comboAccountsList->currentIndex();
    selectedAccount = accountsList->at(selectedAccountIndex);

    qDebug()<<"~ Account \t"<<::xaccAccountGetName(selectedAccount);
    gSplitList = ::xaccAccountGetSplitList(selectedAccount);
    qDebug()<<"~ Splits \t"<<g_list_length(gSplitList);
    g_list_foreach(gSplitList, splits_func, NULL);
    //g_list_free(gSplitList);

    //unifiedColCell GroupBox Manipulations

    /*int rowNum=2;
    foreach(unifiedColCell in gSplitList)
    {
        gridFPO->addWidget(unifiedColCell, rowNum, 0);
        rowNum++;
    }*/
}

} // END namespace gnc

