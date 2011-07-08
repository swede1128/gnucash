#include "ViewletView.hpp"

namespace gnc
{

ViewletView::ViewletView(QWidget * parent, QVBoxLayout * FPOlayout) :
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

    if(comboAccountsList->currentIndex())
    {
        selectedAccountIndex = comboAccountsList->currentIndex();
        selectedAccount = accountsList->at(selectedAccountIndex);
        viewletWidgetsQueue = viewletModel->createViewlet(selectedAccount);
        const int n = viewletWidgetsQueue.count();

        for (int i = 0; i < n; ++i) {
            viewletDisplayLayout->addWidget(viewletWidgetsQueue[i]);
        }
    }
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

    foreach (QWidget *w, viewletWidgetsQueue)
    {
        w->hide();
        viewletDisplayLayout->removeWidget(w);
    }

    viewletWidgetsQueue = viewletModel->createViewlet(selectedAccount);
    const int n = viewletWidgetsQueue.count();

    for (int i = 0; i < n; ++i) {
        viewletDisplayLayout->addWidget(viewletWidgetsQueue[i]);
    }
}

} // END namespace gnc

