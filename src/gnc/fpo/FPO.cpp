#include "FPO.hpp"
#include "gnc/dashboard.hpp"
#include "gnc/mainwindow.hpp"

#include <QLabel>

namespace gnc
{

FPO::FPO(QWidget *parent, QHBoxLayout *FPOLayout) :
    QWidget(parent)
{
    /* For now, create one viewlet; hardcoded. */
    oneViewlet = new ViewletView(parent, FPOLayout);

    /* Test another viewlet */
    anotherViewlet = new ViewletView(parent, FPOLayout);
}

} // END namespace gnc
