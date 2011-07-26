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

    void updateViewlet(::Account * selectedAccount);

    struct structDefaultViewletEntries
    {
        QDate txnDate;
        QString splitAccount;
        QString txnDescription;
        QString splitAmount;
    };
    structDefaultViewletEntries tempEntry;

    QQueue<structDefaultViewletEntries> queueDefaultEntries;
};

} // END namespace gnc

#endif // VIEWLETMODEL_HPP
