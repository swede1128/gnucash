#ifndef VIEWLETVIEW_HPP
#define VIEWLETVIEW_HPP

#include "config.h"

extern "C"
{
#include "qof.h"
#include "engine/Account.h"
#include "engine/Transaction.h"
}

#include "gnc/mainwindow.hpp"
#include "gnc/AccountItemModel.hpp"

#include <QtCore>
#include <QAbstractItemModel>
#include <QtGui>
#include <QWidget>

namespace gnc
{
class ViewletView : public QWidget
{
    Q_OBJECT
public:
    explicit ViewletView(QWidget *parent = 0);
    QVBoxLayout *viewletLayout;
    void loadAccountsTreeComboBox(AccountListModel * const m_accountsListModel);

signals:

public slots:

private:
    AccountListModel *accountsList;

    /* UI Widgets */
    QPushButton *btnSelectAccount;
    QComboBox *comboAccountsList;

    /* Layouts */
    QGridLayout *gridFPO;

    QWidget *groupSelection;
    QVBoxLayout *vSelectionLayout;

    QGroupBox *colMiniJournalFirst;
    QVBoxLayout *vColLayout;


    /** @todo */
    SplitList *gSplitList;
    ::Account *selectedAccount;
    int selectedAccountIndex;

    /* Methods */
    void setViewlet(QWidget *parent);
    static void splits_func(gpointer data, gpointer user_data);

private slots:
    void on_btnSelectAccount_clicked();
};

} // END namespace gnc

#endif // VIEWLETVIEW_HPP
