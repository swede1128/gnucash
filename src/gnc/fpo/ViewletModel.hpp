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
};

} // END namespace gnc

#endif // VIEWLETMODEL_HPP
