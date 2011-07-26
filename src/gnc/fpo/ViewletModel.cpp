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
    Account account(selectedAccount);

    SplitList * splitL = static_cast<SplitList *>(::xaccAccountGetSplitList(selectedAccount));
    SplitQList splitList = Split::fromGList(splitL);
    int numOfSplits = splitList.count();
    Split split;    
    //QDate storedDate;
    int i;

    /* Second implementation
       Now only build up data for the viewlet. Offload the widget
       generation to the View.
    */
    for (i = 0; i < numOfSplits; i++)
    {
        split = splitList.at(i);
        Transaction trans = split.getParent();

        structDefaultViewletEntries entry;

        entry.txnDate = trans.getDatePosted();
        entry.splitAccount = split.getCorrAccountName();
        entry.txnDescription = trans.getDescription();

        queueDefaultEntries.enqueue(entry);

        /* used by view */
        structDefaultViewletEntries tempEntry = queueDefaultEntries.at(i);

        qDebug()<<"Account: "<<tempEntry.splitAccount;
        qDebug()<<"Description: "<<tempEntry.txnDescription;
    }
}

} // END namespace gnc
