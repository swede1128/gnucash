#include "ViewletView.hpp"

namespace gnc
{

ViewletView::ViewletView(QWidget *parent, QVBoxLayout *FPOlayout) :
    QWidget(parent)
{
    setViewlet(parent, FPOlayout);

    viewletModel = new ViewletModel();

    connect(btnSelectAccount, SIGNAL(clicked()),
            this, SLOT(on_btnSelectAccount_clicked()));
}

void
ViewletView::setViewlet(QWidget *parent, QVBoxLayout *FPOLayout)
{
    /* Initialize widgets to defaults */
    comboAccountsList = new QComboBox();
    comboAccountsList->addItem(tr("-NA-"));
    btnSelectAccount = new QPushButton(tr("Select Account"));

    /* 1) Account selection feature of the viewlet */
    FPOLayout->addWidget(comboAccountsList);
    FPOLayout->addWidget(btnSelectAccount);

    /* 2) The actual viewlet display of account(s) data */
    viewletDisplay = new QWidget();
    viewletDisplayLayout = new QVBoxLayout();
    viewletScrollArea = new QScrollArea();

    viewletScrollArea->setWidget(viewletDisplay);
    viewletScrollArea->setAlignment(Qt::AlignLeft);
    viewletScrollArea->setWidgetResizable(true);
    viewletDisplay->setLayout(viewletDisplayLayout);
    FPOLayout->addWidget(viewletScrollArea);

    /*
    // test scroll area
    for(int i=0; i<50; i++)
    {
        QLabel *newrecon = new QLabel("Hello", viewletDisplay);
        //newrecon->setFixedSize(100, 100);
        viewletDisplayLayout->addWidget(newrecon);
    }
    */
}

void
ViewletView::loadAccountsTreeComboBox(AccountListModel * const m_accountsListModel)
{
    accountsList = m_accountsListModel;
    comboAccountsList->setModel(accountsList);
}

/***** Slots *****/

void
ViewletView::on_btnSelectAccount_clicked()
{
    selectedAccountIndex = comboAccountsList->currentIndex();
    selectedAccount = accountsList->at(selectedAccountIndex);

    pSplitList = ::xaccAccountGetSplitList(selectedAccount);

    SplitQList newSplits = Split::fromGList(pSplitList);
    int num = newSplits.count();
    ::Split * split;
    //Split  * oneSplit;
    for (int i = 0; i < num; i++)
    {
        split = (::Split *)newSplits.at(i);
        //oneSplit = (Split *)newSplits.at(i);
        //oneSplit->getMemo();

        QDateEdit * editSplitDate = new QDateEdit();
        editSplitDate->setDate(viewletModel->getDatePosted(split));
        viewletDisplayLayout->addWidget(editSplitDate);

        QLabel * editAccountName = new QLabel(
                    viewletModel->getAccountName(split));
        viewletDisplayLayout->addWidget(editAccountName);

        QLabel * editDescription = new QLabel("~    Description: "+
                    viewletModel->getDescription(split));
        viewletDisplayLayout->addWidget(editDescription);


        /*QLineEdit * editReconcileStatus = new QLineEdit(
                    viewletModel->getReconciliationStatus(split));
        viewletDisplayLayout->addWidget(editReconcileStatus);*/
    }
}

} // END namespace gnc

