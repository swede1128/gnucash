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
    explicit ViewletView(QWidget * parent = 0, QHBoxLayout * FPOLayout = NULL);
    void loadAccountsTreeComboBox(AccountListModel * const m_accountsListModel);

signals:

public slots:

private:
    ViewletModel * viewletModel;
    AccountListModel * accountsList;

    /* UI Widgets */    
    QWidget * viewletDisplay;
    QQueue<QWidget *> datesQueue;
    QQueue<QWidget *> accountsQueue;
    QQueue<QWidget *> descQueue;

    QVBoxLayout * viewletDisplayLayout;
    QScrollArea * viewletScrollArea;

    QComboBox * comboAccountsList;

    /** @todo */
    SplitList * pSplitList;
    ::Account * selectedAccount;
    int selectedAccountIndex;

    /* Methods */
    void setViewlet(QWidget * parent, QHBoxLayout * FPOLayout);
    void createViewlet();
    void getViewletQueues();
    void drawViewletLayout();
    void removeViewletWidgets();

private slots:
    void updateViewlet();
};

} // END namespace gnc

#endif // VIEWLETVIEW_HPP
