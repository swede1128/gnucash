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



    //depre
    struct structViewletEntries
    {
        QQueue<QWidget *> datesQueue;
        //QQueue<bool> isNewDateQueue;
        QQueue<QWidget *> accountsQueue;
        QQueue<QWidget *> descQueue;
        QQueue<QWidget *> splitAmountQueue;
    } viewletEntries;

    struct structDefaultViewletEntries
    {
        QDate txnDate;
        QString splitAccount;
        QString txnDescription;
        QString splitAmount;
    };
    structDefaultViewletEntries tempEntry;

    QQueue<structDefaultViewletEntries> queueDefaultEntries;

private:
    //depre
    QString getAccountName(::Split * split) {
        return QString::fromUtf8(::xaccSplitGetCorrAccountName(split)); }
    QString getReconciliationStatus(::Split * split) {
        return static_cast< QString const>(::xaccSplitGetReconcile(split)); }
    QString getDescription(::Split * split) {
        ::Transaction *txn = xaccSplitGetParent(split);
        return QString::fromUtf8(::xaccTransGetDescription(txn)); }
    QDate getDatePosted(::Split * split) {
        ::Transaction *txn = ::xaccSplitGetParent(split);
        return toQDate(::xaccTransGetDatePostedGDate(txn)); }
    inline QDate toQDate(const ::GDate& d)
    {
        if (g_date_valid(&d))
            return QDate(g_date_year(&d), g_date_month(&d), g_date_day(&d));
        else
            return QDate();
    }
};

} // END namespace gnc

#endif // VIEWLETMODEL_HPP
