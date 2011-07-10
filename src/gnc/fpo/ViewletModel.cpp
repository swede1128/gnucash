#include "ViewletModel.hpp"

namespace gnc
{

ViewletModel::ViewletModel()
{
}

void
ViewletModel::updateViewlet(::Account * selectedAccount)
{
    ::SplitList * C_SplitList = ::xaccAccountGetSplitList(selectedAccount);

    SplitQList splitList = Split::fromGList(C_SplitList);
    int numOfSplits = splitList.count();
    ::Split * C_split;
    for (int i = 0; i < numOfSplits; i++)
    {
        C_split = static_cast< ::Split*>(splitList.at(i));

        QDateEdit * editSplitDate = new QDateEdit();
        editSplitDate->setDate(getDatePosted(C_split));
        datesQueue.enqueue(editSplitDate);

        QLabel * editAccountName = new QLabel();
        editAccountName->setText(getAccountName(C_split));
        accountsQueue.enqueue(editAccountName);

        QLabel * editDescription = new QLabel();
        editDescription->setText("~    Description: " + getDescription(C_split));
        descQueue.enqueue(editDescription);


            /*QLineEdit * editReconcileStatus = new QLineEdit(
                        viewletModel->getReconciliationStatus(split));
            viewletDisplayLayout->addWidget(editReconcileStatus);*/
    }
}

} // END namespace gnc
