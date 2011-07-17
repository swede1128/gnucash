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
    QDate storedDate;
    int i;


    for (i = 0, storedDate.setYMD(1980,1,1); i < numOfSplits; i++)
    {
        split = splitList.at(i);
        Transaction trans = split.getParent();

        QDateEdit * editSplitDate = new QDateEdit();
        editSplitDate->setDate(trans.getDatePosted());
        editSplitDate->setEnabled(FALSE);
        viewletEntries.datesQueue.enqueue(editSplitDate);
        //viewletEntries.isNewDateQueue.enqueue(FALSE);

        /*
        if( editSplitDate->date() > storedDate)
        {
            qDebug() << "New date!! Show its widget.";
            viewletEntries.isNewDateQueue.enqueue(TRUE);
            storedDate = trans.getDatePosted();
        }
        */

        QLabel * editAccountName = new QLabel();
        editAccountName->setText(split.getCorrAccountName());
        viewletEntries.accountsQueue.enqueue(editAccountName);

        /* get the amount of the split from its Numeric */
        Numeric gncAmt;
        gncAmt = split.getAmount();
        double numer;
        int denom;
        numer = gncAmt.num();
        denom = gncAmt.denom();
        double amt = numer/denom;

        /*
        QLabel * editLabel = new QLabel();
        editLabel->setText(QString::number(amt));
        viewletEntries.splitAmountQueue.enqueue(editLabel);
        */

        /* temporary output; probably a separate widget for amount
        with correct currency for the split in future */
        QLabel * editDescription = new QLabel();
        editDescription->setText(trans.getDescription() + " ($ " + QString::number(amt) + ")");
        viewletEntries.descQueue.enqueue(editDescription);
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
