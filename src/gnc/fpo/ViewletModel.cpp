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

    /* first implementation */
    for (i = 0; i < numOfSplits; i++)
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

        /* get all transactions earlier than the specified date */
        QofQuery *qr =  qof_query_create_for (GNC_ID_SPLIT);
        qof_query_set_book(qr, ::gnc_account_get_book(selectedAccount));
        // Query for transactions
        QofQuery *txn_query = qof_query_create_for (GNC_ID_TRANS);
        // To look for dates, you need to create a "PredData" object
        Timespec calve_date;
        calve_date =  gdate_to_timespec(trans.getGDatePosted());
        QofQueryPredData *pred_data = qof_query_date_predicate (QOF_COMPARE_LTE,
                                              QOF_DATE_MATCH_NORMAL,
                                              calve_date);
        // and additionally a "query parameter" object
        GSList *param_list = qof_query_build_param_list (TRANS_DATE_POSTED, NULL);
        // The "PredData" and the "query parameter" object are added to this query
        qof_query_add_term (txn_query, param_list, pred_data,
                            QOF_QUERY_FIRST_TERM);

        // Query is run; result is returned
        GList *result =  qof_query_run (qr);

        SplitQList querySplitList = Split::fromGList(result);
        Split qSplit = querySplitList.at(i);
        //qDebug()<< qSplit.getCorrAccountName();

        // "result" is now a GList of "Transaction*" because at
        //qof_query_create_for, we've asked for transactions.

        // When finished, delete the QofQuery object but this will
        // also delete the "result" list.
        qof_query_destroy (txn_query);
    }

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
