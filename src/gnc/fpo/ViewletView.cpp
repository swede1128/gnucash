#include "ViewletView.hpp"

namespace gnc
{

ViewletView::ViewletView(QWidget * parent, QHBoxLayout * FPOlayout) :
    QWidget(parent)
{
    viewletModel = new ViewletModel();

    setViewlet(parent, FPOlayout);

    connect(comboAccountsList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateViewlet()));
}

void
ViewletView::setViewlet(QWidget *parent, QHBoxLayout *FPOLayout)
{
    /* Initialize widgets to defaults */
    comboAccountsList = new QComboBox();
    comboAccountsList->addItem(tr("-NA-"));

    QWidget *viewletContainer = new QWidget;
    FPOLayout->addWidget(viewletContainer);

    QVBoxLayout *vLay = new QVBoxLayout;
    viewletContainer->setLayout(vLay);

    /* 1) Account selection feature of the viewlet */
    vLay->addWidget(comboAccountsList);

    /* 2) The actual viewlet display of account(s) data */
    viewletDisplay = new QWidget();
    viewletDisplayLayout = new QVBoxLayout();
    viewletScrollArea = new QScrollArea();

    viewletScrollArea->setWidget(viewletDisplay);
    viewletScrollArea->setAlignment(Qt::AlignLeft);
    viewletScrollArea->setWidgetResizable(true);
    viewletDisplay->setLayout(viewletDisplayLayout);
    vLay->addWidget(viewletScrollArea);

    createViewlet();
}

void
ViewletView::loadAccountsTreeComboBox(AccountListModel * const m_accountsListModel)
{
    accountsList = m_accountsListModel;
    comboAccountsList->setModel(accountsList);
}

void
ViewletView::createViewlet()
{
    if(comboAccountsList->currentIndex())
    {
        selectedAccountIndex = comboAccountsList->currentIndex();
        selectedAccount = accountsList->at(selectedAccountIndex);

        getViewletQueues();
        drawViewletLayout();
    }
}

void
ViewletView::getViewletQueues()
{
    viewletModel->updateViewlet(selectedAccount);
    datesQueue = viewletModel->datesQueue;
    accountsQueue = viewletModel->accountsQueue;
    descQueue = viewletModel->descQueue;
}

void
ViewletView::drawViewletLayout()
{
    int numOfDates = datesQueue.count();
    for (int i = 0; i < numOfDates; ++i)
    {
        viewletDisplayLayout->addWidget(datesQueue[i]);
        datesQueue[i]->setObjectName("dateWidget");
        viewletDisplayLayout->addWidget(accountsQueue[i]);
        accountsQueue[i]->setObjectName("accountWidget");
        viewletDisplayLayout->addWidget(descQueue[i]);
        descQueue[i]->setObjectName("descWidget");
    }
}

void
ViewletView::removeViewletWidgets()
{
    foreach (QWidget *w, datesQueue)
    {
        w->hide();
        viewletDisplayLayout->removeWidget(w);
    }
    foreach (QWidget *w, accountsQueue)
    {
        w->hide();
        viewletDisplayLayout->removeWidget(w);
    }
    foreach (QWidget *w, descQueue)
    {
        w->hide();
        viewletDisplayLayout->removeWidget(w);
    }
}

/***** Slots *****/

void
ViewletView::updateViewlet()
{
    selectedAccountIndex = comboAccountsList->currentIndex();
    selectedAccount = accountsList->at(selectedAccountIndex);

    removeViewletWidgets();
    getViewletQueues();
    drawViewletLayout();
}

} // END namespace gnc

