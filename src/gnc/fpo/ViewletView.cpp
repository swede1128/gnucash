#include "ViewletView.hpp"

namespace gnc
{

ViewletView::ViewletView(QWidget * parent, QHBoxLayout * FPOlayout) :
    QWidget(parent)
{
    viewletModel = new ViewletModel();
}

/***** Public *****/

void
ViewletView::defaultVSet(QWidget *parent, QHBoxLayout *FPOLayout)
{
    /* For default viewlet */
    comboAccountsList = new QComboBox();
    comboAccountsList->addItem(tr("-NA-"));

    connect(comboAccountsList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(defaultVUpdate()));

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
    QWidget *defaultViewletWidget = new QWidget();
    defaultVLayout = new QVBoxLayout();
    QScrollArea *viewletScrollArea = new QScrollArea();

    viewletScrollArea->setWidget(defaultViewletWidget);
    viewletScrollArea->setAlignment(Qt::AlignLeft);
    viewletScrollArea->setWidgetResizable(true);
    defaultViewletWidget->setLayout(defaultVLayout);
    vLay->addWidget(viewletScrollArea);

    //create viewlet
    if(comboAccountsList->currentIndex())
    {
        selectedAccountIndex = comboAccountsList->currentIndex();
        selectedAccount = accountsList->at(selectedAccountIndex);

        defaultVDraw();
    }
}

/***** Private *****/

/** Create the widgets for the viewlet entries

    Passes the selected account to the model. The updated textual
    data in the struct of the model is used in the newly created
    widgets.
*/
void
ViewletView::defaultVDraw()
{
    /* Update the struct in ViewletModel with data from the selected
       account
    */
    viewletModel->updateViewlet(selectedAccount);

    int numOfTransactions = viewletModel->queueEntries.count();
    for (int i = 0; i < numOfTransactions; ++i)
    {
        viewletModel->tempEntry = viewletModel->queueEntries.at(i);

        txnDescription = viewletModel->tempEntry.txnDescription;
        setLabel(txnDescription, "descWidget", defaultVLayout);

        splitAccount = viewletModel->tempEntry.splitAccount;
        setLabel(splitAccount, "accountWidget", defaultVLayout);

#if 0
        QLabel *lblTxnDescription = new QLabel();
        lblTxnDescription->setText(viewletModel->tempEntry.txnDescription);
        defaultVLayout->addWidget(lblTxnDescription);
        /* Use this QList to store all the widgets generated for
           this account, so when the user chooses a different
           account, a new viewlet could be drawn by first removing
           these old widgets.
        */
        viewletWidgetsList.append(lblTxnDescription);

        lblTxnDescription->setObjectName("descWidget");
#endif


        qDebug()<<"for loop iter "<<viewletWidgetsList.count();
    }
}

void
ViewletView::defaultVRemoveWidgets()
{
    /* Remove old widgets */
    int numOfWidgets = viewletWidgetsList.count();
    for (int i=0; i<numOfWidgets; ++i)
    {
        QWidget *wdg = viewletWidgetsList.at(i);
        delete wdg;
    }

    /* Empty the data structures */
    viewletModel->queueEntries.clear();
    viewletWidgetsList.clear();
}

/**********/

void
ViewletView::setLabel(QString data, QString objectName, QVBoxLayout *layout)
{
    QLabel *lbl = new QLabel();
    lbl->setText(data);
    layout->addWidget(lbl);
    /* Used as CSS ID by QStyleSheet */
    lbl->setObjectName(objectName);
    /* Use this list to store all the widgets generated for
       this viewlet, so when the viewlet updates, a new viewlet could
       be drawn by first removing the old widgets in this list.
    */
    viewletWidgetsList.append(lbl);
}

void
ViewletView::loadAccountsTreeComboBox(AccountListModel * const m_accountsListModel)
{
    accountsList = m_accountsListModel;
    comboAccountsList->setModel(accountsList);
}

/***** Slots *****/

void
ViewletView::defaultVUpdate()
{
    selectedAccountIndex = comboAccountsList->currentIndex();
    selectedAccount = accountsList->at(selectedAccountIndex);

    defaultVRemoveWidgets();
    defaultVDraw();
}

} // END namespace gnc

