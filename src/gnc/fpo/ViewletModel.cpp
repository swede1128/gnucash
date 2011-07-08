#include "ViewletModel.hpp"

namespace gnc
{

ViewletModel::ViewletModel()
{
}

QQueue<QWidget *>
ViewletModel::createViewlet(::Account * selectedAccount)
{
    ::SplitList * C_SplitList = ::xaccAccountGetSplitList(selectedAccount);

    SplitQList splitList = Split::fromGList(C_SplitList);
    int numOfSplits = splitList.count();

    ::Split * split;
    QQueue<QWidget *> viewletWidgetsQueue;
    for (int i = 0; i < numOfSplits; i++)
    {
        split = (::Split *)splitList.at(i);

        QDateEdit * editSplitDate = new QDateEdit();
        editSplitDate->setDate(getDatePosted(split));
        viewletWidgetsQueue.enqueue(editSplitDate);
    /*
            QLabel * editAccountName = new QLabel(
                        viewletModel->getAccountName(split));
            viewletDisplayLayout->addWidget(editAccountName);

            QLabel * editDescription = new QLabel("~    Description: "+
                        viewletModel->getDescription(split));
            viewletDisplayLayout->addWidget(editDescription);
    */

            /*QLineEdit * editReconcileStatus = new QLineEdit(
                        viewletModel->getReconciliationStatus(split));
            viewletDisplayLayout->addWidget(editReconcileStatus);*/
    }
    return viewletWidgetsQueue;
}

} // END namespace gnc
