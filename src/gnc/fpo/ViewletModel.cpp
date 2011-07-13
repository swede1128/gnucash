#include "ViewletModel.hpp"
#include "gnc/Transaction.hpp"
#include "gnc/Split.hpp"
#include "gnc/SplitListModel.hpp"
#include "gnc/Numeric.hpp"

namespace gnc
{

ViewletModel::ViewletModel()
{
}

void
ViewletModel::updateViewlet(::Account * selectedAccount)
{
    Account * account;
    account =  reinterpret_cast<Account *>(selectedAccount);

    SplitList * splitL = static_cast<SplitList *>(::xaccAccountGetSplitList(selectedAccount));
    SplitQList splitList = Split::fromGList(splitL);
    int numOfSplits = splitList.count();
    Split split;
    for (int i = 0; i < numOfSplits; i++)
    {
        split = splitList.at(i);
        Transaction trans = split.getParent();

        QDateEdit * editSplitDate = new QDateEdit();
        editSplitDate->setDate(trans.getDatePosted());
        datesQueue.enqueue(editSplitDate);

        QLabel * editAccountName = new QLabel();
        editAccountName->setText(split.getCorrAccountName());
        accountsQueue.enqueue(editAccountName);

        QLabel * editDescription = new QLabel();
        editDescription->setText("~    Description: " + trans.getDescription());
        descQueue.enqueue(editDescription);
    }

#if 0
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
#endif
}

} // END namespace gnc
