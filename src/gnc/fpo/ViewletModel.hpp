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
    QString getAccountName(::Split * split) {
        return (QString const) ::xaccSplitGetCorrAccountName(split); }
    QString getReconciliationStatus(::Split * split) {
        return (QString const) ::xaccSplitGetReconcile(split); }

    inline QDate toQDate(const ::GDate& d)
    {
        if (g_date_valid(&d))
            return QDate(g_date_year(&d), g_date_month(&d), g_date_day(&d));
        else
            return QDate();
    }
    QString getDescription(::Split * split) {
        ::Transaction *txn = xaccSplitGetParent(split);
        return (QString const) xaccTransGetDescription(txn); }

    QQueue<QWidget *> createViewlet(::Account * selectedAccount);

private:

    QDate getDatePosted(::Split * split) {
        ::Transaction *txn = xaccSplitGetParent(split);
        return toQDate(::xaccTransGetDatePostedGDate(txn)); }

};

} // END namespace gnc

#endif // VIEWLETMODEL_HPP
