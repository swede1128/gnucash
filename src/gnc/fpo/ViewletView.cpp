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
    /** @bugid_1 1) Account selection feature of the viewlet  */
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

        drawViewlet();
    }
}

void
ViewletView::loadAccountsTreeComboBox(AccountListModel * const m_accountsListModel)
{
    accountsList = m_accountsListModel;
    comboAccountsList->setModel(accountsList);
}

void
ViewletView::removeViewletWidgets()
{
    //LEFT OFF HERE work on the iterator to make removing widgets workale
    int numOfWidgets = viewletWidgetsList.count();
    qDebug() <<numOfWidgets;

    for (int i=0; i<numOfWidgets; ++i)
    {
        QWidget *wdg = viewletWidgetsList.at(i);
        wdg->hide();
        viewletDisplayLayout->removeWidget(wdg);
    }
    viewletWidgetsList.clear();
    viewletWidgetsList.removeAt(0);

    qDebug() <<"After clear " <<numOfWidgets;
    /*
    foreach (QWidget *w, viewletWidgetsList)
    {
        viewletWidgetsList.dequeue(w);
        //w->hide();
        //viewletDisplayLayout->removeWidget(w);
    }*/
}


/***** Slots *****/

void
ViewletView::updateViewlet()
{
    selectedAccountIndex = comboAccountsList->currentIndex();
    selectedAccount = accountsList->at(selectedAccountIndex);

    removeViewletWidgets();

    drawViewlet();

}

void
ViewletView::drawViewlet()
{
    /* Update the queueDefaultEntries struct in ViewletModel with new data */
    viewletModel->updateViewlet(selectedAccount);

    int numOfTransactions = viewletModel->queueDefaultEntries.count();
    for (int i = 0; i < numOfTransactions; ++i)
    {
        /* Description */
        viewletModel->tempEntry = viewletModel->queueDefaultEntries.at(i);
        QLabel *lblTxnDescription = new QLabel();
        lblTxnDescription->setText(viewletModel->tempEntry.txnDescription);
        viewletDisplayLayout->addWidget(lblTxnDescription);
        /* Use this QList to store all the widgets generated for
           this account, so when the user chooses a different
           account, a new viewlet could be drawn by first removing
           these old widgets.
        */
        viewletWidgetsList.append(lblTxnDescription);
        /* Used as ID by QStyleSheet */
        lblTxnDescription->setObjectName("descWidget");
    }
}

} // END namespace gnc

