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

        processViewlet();
        drawViewletLayout();
    }
}

void
ViewletView::processViewlet()
{
    viewletModel->updateViewlet(selectedAccount);
}

void
ViewletView::drawViewletLayout()
{
    int numOfDates = viewletModel->viewletEntries.datesQueue.count();
    for (int i = 0; i < numOfDates; ++i)
    {
        viewletDisplayLayout->addWidget(viewletModel->viewletEntries.datesQueue[i]);
        viewletModel->viewletEntries.datesQueue[i]->setObjectName("dateWidget");
        viewletDisplayLayout->addWidget(viewletModel->viewletEntries.accountsQueue[i]);
        viewletModel->viewletEntries.accountsQueue[i]->setObjectName("accountWidget");
        viewletDisplayLayout->addWidget(viewletModel->viewletEntries.descQueue[i]);
        viewletModel->viewletEntries.descQueue[i]->setObjectName("descWidget");
        /*
        viewletDisplayLayout->addWidget(viewletModel->viewletEntries.splitAmountQueue[i]);
        viewletModel->viewletEntries.splitAmountQueue[i]->setObjectName("splitAmountWidget");
        */
    }
}

void
ViewletView::removeViewletWidgets()
{
    foreach (QWidget *w, viewletModel->viewletEntries.datesQueue)
    {
            w->hide();
            viewletDisplayLayout->removeWidget(w);
    }
    foreach (QWidget *w, viewletModel->viewletEntries.accountsQueue)
    {
        w->hide();
        viewletDisplayLayout->removeWidget(w);
    }
    foreach (QWidget *w, viewletModel->viewletEntries.descQueue)
    {
        w->hide();
        viewletDisplayLayout->removeWidget(w);
    }
    /*
    foreach (QWidget *w, viewletModel->viewletEntries.splitAmountQueue)
    {
        w->hide();
        viewletDisplayLayout->removeWidget(w);
    }
    */
}

/***** Slots *****/

void
ViewletView::updateViewlet()
{
    selectedAccountIndex = comboAccountsList->currentIndex();
    selectedAccount = accountsList->at(selectedAccountIndex);

    removeViewletWidgets();
    processViewlet();
    drawViewletLayout();
}

} // END namespace gnc

