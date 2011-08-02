#ifndef VIEWLETMODEL_HPP
#define VIEWLETMODEL_HPP

#include <QtCore>
#include <QtGui>

#include "config.h"

extern "C"
{
#include "qof.h"
#include "engine/Account.h"
#include "engine/Transaction.h"
#include "engine/Split.h"
}

#include "gnc/Split.hpp"

namespace gnc
{

class ViewletModel
{
public:
    ViewletModel();
    void defaultVGenerate(::Account * selectedAccount);

    struct structViewletEntries
    {
        QString txnDate;
        bool isDateEqual;
        QString splitAccount;
        bool isSplitAccountEqual;
        QString txnDescription;
        QString splitAmount;
    };
    structViewletEntries tempEntry;
    QQueue<structViewletEntries> queueEntries;

private:
    SplitQList buildSplitListDateSort(::Account *selectedAccount);
    void buildMiniJournalStruct(SplitQList splitList);

};

} // END namespace gnc

#endif // VIEWLETMODEL_HPP
