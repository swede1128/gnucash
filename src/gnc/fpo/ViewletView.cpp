#include "ViewletView.hpp"

namespace gnc
{

ViewletView::ViewletView(QWidget * parent, QHBoxLayout * FPOlayout) :
    QWidget(parent)
{
    viewletModel = new ViewletModel();

    /* For default viewlet */
    comboAccountsList = new QComboBox();
    comboAccountsList->addItem(tr("-NA-"));

    connect(comboAccountsList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateViewlet()));
}

void
ViewletView::setDefaultViewlet(QWidget *parent, QHBoxLayout *FPOLayout)
{
    /* Add a new QWidget (acts as a container for this viewlet) to the
       layout of QWidget (QDockWidget>QWidget, i.e, dockwFPO>dockcFPO)
       in dashboard QMainWindow.
    */
    QWidget *viewletContainer = new QWidget;
    FPOLayout->addWidget(viewletContainer);

    /* Set a layout for the container QWidget */
    QVBoxLayout *vLay = new QVBoxLayout;
    viewletContainer->setLayout(vLay);

    /***** Start of viewlet specific implementations *****/
    /* Specification:
       This default viewlet contains two widgets, 1) An account
       selection widget, and 2) A scroll area which wraps a QWidget
       to show the entries.*/
    /** @combofix 1) Account selection feature of the viewlet  */
    /*
    comboAccountsList = new QComboBox();
    comboAccountsList->addItem(tr("-NA-"));
    */

    vLay->addWidget(comboAccountsList);

    /* 2) The actual viewlet display of account selected in 1) */
    QWidget *viewletDisplay = new QWidget();
    viewletDisplayLayout = new QVBoxLayout();
    QScrollArea *viewletScrollArea = new QScrollArea();

    viewletScrollArea->setWidget(viewletDisplay);
    viewletScrollArea->setAlignment(Qt::AlignLeft);
    viewletScrollArea->setWidgetResizable(true);
    viewletDisplay->setLayout(viewletDisplayLayout);
    vLay->addWidget(viewletScrollArea);

    //create viewlet
    if(comboAccountsList->currentIndex())
    {
        selectedAccountIndex = comboAccountsList->currentIndex();
        selectedAccount = accountsList->at(selectedAccountIndex);

        // processViewlet
        viewletModel->updateViewlet(selectedAccount);

        // drawviewlet
        int numOfDates = viewletModel->queueDefaultEntries.count();
        for (int i = 0; i < numOfDates; ++i)
        {
            //structDefaultViewletEntries tempEntry = viewletModel->queueDefaultEntries.at(i);
            viewletModel->tempEntry = viewletModel->queueDefaultEntries.at(i);
            QLabel *lblTxnDescription = new QLabel();

            lblTxnDescription->setText(viewletModel->tempEntry.txnDescription);
            viewletDisplayLayout->addWidget(lblTxnDescription);
            lblTxnDescription->setObjectName("descWidget");

           //LEFT OFF HERE now work on removeViewletWidgets

            /*
            viewletDisplayLayout->addWidget(viewletModel->viewletEntries.accountsQueue[i]);
            viewletModel->viewletEntries.accountsQueue[i]->setObjectName("accountWidget");
            viewletDisplayLayout->addWidget(viewletModel->viewletEntries.descQueue[i]);
            viewletModel->viewletEntries.descQueue[i]->setObjectName("descWidget");
            */
            /*
            viewletDisplayLayout->addWidget(viewletModel->viewletEntries.splitAmountQueue[i]);
            viewletModel->viewletEntries.splitAmountQueue[i]->setObjectName("splitAmountWidget");
            */
        }
    }
}

void
ViewletView::loadAccountsTreeComboBox(AccountListModel * const m_accountsListModel)
{
    accountsList = m_accountsListModel;
    comboAccountsList->setModel(accountsList);
}

#if 0
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
#endif


/***** Slots *****/

void
ViewletView::updateViewlet()
{
    selectedAccountIndex = comboAccountsList->currentIndex();
    selectedAccount = accountsList->at(selectedAccountIndex);

    //removeViewletWidgets();

    // processViewlet
    viewletModel->updateViewlet(selectedAccount);

    // drawviewlet

    int numOfDates = viewletModel->queueDefaultEntries.count();
    for (int i = 0; i < numOfDates; ++i)
    {
        //structDefaultViewletEntries tempEntry = viewletModel->queueDefaultEntries.at(i);
        viewletModel->tempEntry = viewletModel->queueDefaultEntries.at(i);

        QLabel *lblTxnDescription = new QLabel();
        lblTxnDescription->setText(viewletModel->tempEntry.txnDescription);
        viewletDisplayLayout->addWidget(lblTxnDescription);
        lblTxnDescription->setObjectName("descWidget");

       //LEFT OFF HERE now work on removeViewletWidgets

        /*
        viewletDisplayLayout->addWidget(viewletModel->viewletEntries.accountsQueue[i]);
        viewletModel->viewletEntries.accountsQueue[i]->setObjectName("accountWidget");
        viewletDisplayLayout->addWidget(viewletModel->viewletEntries.descQueue[i]);
        viewletModel->viewletEntries.descQueue[i]->setObjectName("descWidget");
        */
        /*
        viewletDisplayLayout->addWidget(viewletModel->viewletEntries.splitAmountQueue[i]);
        viewletModel->viewletEntries.splitAmountQueue[i]->setObjectName("splitAmountWidget");
        */
    }
}

} // END namespace gnc

