#ifndef VIEWLETVIEW_HPP
#define VIEWLETVIEW_HPP

#include "config.h"

extern "C"
{
#include "qof.h"
#include "engine/Account.h"
#include "engine/Transaction.h"
#include "engine/Split.h"
}

#include "gnc/mainwindow.hpp"
#include "gnc/fpo/ViewletModel.hpp"
#include "gnc/AccountItemModel.hpp"
#include "gnc/SplitListModel.hpp"

#include <QtCore>
#include <QAbstractItemModel>
#include <QtGui>
#include <QWidget>

namespace gnc
{
class ViewletModel;

class ViewletView : public QWidget
{
    Q_OBJECT
public:
    explicit ViewletView(QWidget *parent = 0, QVBoxLayout *FPOLayout = NULL);
    QVBoxLayout *viewletLayout;
    void loadAccountsTreeComboBox(AccountListModel * const m_accountsListModel);

signals:

public slots:

private:
    AccountListModel *accountsList;

    /* UI Widgets */
    QComboBox *comboAccountsList;

    ViewletModel *viewletModel;

    QScrollArea *viewletScrollArea;
    QWidget *viewletDisplay;
    QVBoxLayout *viewletDisplayLayout;



    QWidget *groupSelection;
    QVBoxLayout *vSelectionLayout;

    QWidget *colMiniJournalFirst;
    QVBoxLayout *vColLayout;


    /** @todo */
    SplitList *pSplitList;
    ::Account *selectedAccount;
    int selectedAccountIndex;

    /* Methods */
    void setViewlet(QWidget *parent, QVBoxLayout *FPOLayout);

private slots:
    void createViewlet();
};

} // END namespace gnc

#endif // VIEWLETVIEW_HPP
