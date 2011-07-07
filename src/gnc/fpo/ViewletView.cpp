#include "ViewletView.hpp"

namespace gnc
{

ViewletView::ViewletView(QWidget *parent, QVBoxLayout *FPOlayout) :
    QWidget(parent)
{
    setViewlet(parent, FPOlayout);

    viewletModel = new ViewletModel();

    connect(comboAccountsList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(createViewlet()));
}

void
ViewletView::setViewlet(QWidget *parent, QVBoxLayout *FPOLayout)
{
    /* Initialize widgets to defaults */
    comboAccountsList = new QComboBox();
    comboAccountsList->addItem(tr("-NA-"));

    /* 1) Account selection feature of the viewlet */
    FPOLayout->addWidget(comboAccountsList);

    /* 2) The actual viewlet display of account(s) data */
    viewletDisplay = new QWidget();
    viewletDisplayLayout = new QVBoxLayout();
    viewletScrollArea = new QScrollArea();

    viewletScrollArea->setWidget(viewletDisplay);
    viewletScrollArea->setAlignment(Qt::AlignLeft);
    viewletScrollArea->setWidgetResizable(true);
    viewletDisplay->setLayout(viewletDisplayLayout);
    FPOLayout->addWidget(viewletScrollArea);
}

void
ViewletView::loadAccountsTreeComboBox(AccountListModel * const m_accountsListModel)
{
    accountsList = m_accountsListModel;
    comboAccountsList->setModel(accountsList);
}

/***** Slots *****/

void
ViewletView::createViewlet()
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

