#include "FPO.hpp"
#include "gnc/dashboard.hpp"
#include "gnc/mainwindow.hpp"

#include <QLabel>

namespace gnc
{

FPO::FPO(QWidget *parent, QVBoxLayout *FPOLayout) :
    QWidget(parent)
{
    /* For now, create one viewlet; hardcoded. */
    oneViewlet = new ViewletView(parent, FPOLayout);
}

} // END namespace gnc
