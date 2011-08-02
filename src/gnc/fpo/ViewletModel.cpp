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
ViewletModel::defaultVGenerate(::Account *selectedAccount)
{
    SplitQList splitList = buildSplitListDateSort(selectedAccount);
    buildMiniJournalStruct(splitList);
}

#if 0

    /* get all transactions earlier than the specified date */
    QofQuery *qr =  qof_query_create_for (GNC_ID_SPLIT);
    qof_query_set_book(qr, ::gnc_account_get_book(selectedAccount));
    // To look for dates, you need to create a "PredData" object
    Timespec calve_date;

    //calve_date =  gdate_to_timespec(trans.getGDatePosted());
    QofQueryPredData *pred_data = qof_query_date_predicate (QOF_COMPARE_LTE,
                                          QOF_DATE_MATCH_NORMAL,
                                          calve_date);
    // and additionally a "query parameter" object
    GSList *param_list = qof_query_build_param_list (TRANS_DATE_POSTED, NULL);
    // The "PredData" and the "query parameter" object are added to this query
    qof_query_add_term (qr, param_list, pred_data,
                        QOF_QUERY_FIRST_TERM);

    // Query is run; result is returned
    GList *result =  qof_query_run (qr);

    SplitQList querySplitList = Split::fromGList(result);
    Split qSplit;
    int numOfQuerSplits = querySplitList.count();

    for(i=0; i < numOfQuerSplits; ++i)
    {
        qSplit = querySplitList.at(i);
        //qDebug()<<qSplit.getCorrAccountName();
        qDebug()<<qSplit.getParent().getDatePosted().toString();

    }

    // "result" is now a GList of "Transaction*" because at
    //qof_query_create_for, we've asked for transactions.

    // When finished, delete the QofQuery object but this will
    // also delete the "result" list.
    qof_query_destroy (qr);
#endif

SplitQList
ViewletModel::buildSplitListDateSort(::Account *selectedAccount)
{
    SplitList * splitL = static_cast<SplitList *>(::xaccAccountGetSplitList(selectedAccount));
    return Split::fromGList(splitL);
}

void
ViewletModel::buildMiniJournalStruct(SplitQList splitList)
{
    int numOfSplits = splitList.count();
    Split split;
    int i;
    QDate tempDate;
    QString tempAccount;

    for (i = 0; i < numOfSplits; i++)
    {
        split = splitList.at(i);
        Transaction txn = split.getParent();

        structViewletEntries entry;

        if(i == 0)
        {
            tempDate = txn.getDatePosted();
            entry.isDateEqual = false;
            tempAccount = split.getCorrAccountName();
            entry.isSplitAccountEqual = false;
        }
        else
        {
            if(txn.getDatePosted() == tempDate)
            {
                entry.isDateEqual = true;
            }
            else
            {
                entry.isDateEqual = false;
                tempDate = txn.getDatePosted();
            }

            if(split.getCorrAccountName() == tempAccount)
            {
                entry.isSplitAccountEqual = true;
            }
            else
            {
                entry.isSplitAccountEqual = false;
                tempAccount = split.getCorrAccountName();
            }
        }

        entry.txnDate = txn.getDatePosted().toString();
        entry.splitAccount = split.getCorrAccountName();
        entry.txnDescription = txn.getDescription();

        Numeric splitAmount;
        splitAmount = split.getAmount();
        PrintAmountInfo printInfo(split, true);
        entry.splitAmount = splitAmount.printAmount(printInfo);

        queueEntries.enqueue(entry);
    }
}
} // END namespace gnc
