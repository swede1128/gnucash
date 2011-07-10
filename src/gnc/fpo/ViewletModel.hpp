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

    QQueue<QWidget *> datesQueue;
    QQueue<QWidget *> accountsQueue;
    QQueue<QWidget *> descQueue;

private:
    QString getAccountName(::Split * split) {
        return static_cast< QString const>(::xaccSplitGetCorrAccountName(split)); }
    QString getReconciliationStatus(::Split * split) {
        return static_cast< QString const>(::xaccSplitGetReconcile(split)); }
    QString getDescription(::Split * split) {
        ::Transaction *txn = xaccSplitGetParent(split);
        return static_cast< QString const>(::xaccTransGetDescription(txn)); }
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
