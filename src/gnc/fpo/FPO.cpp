#include "FPO.hpp"
#include "gnc/dashboard.hpp"
#include "gnc/mainwindow.hpp"

#include <QLabel>

namespace gnc
{

FPO::FPO(QWidget *parent, QHBoxLayout *FPOLayout) :
    QWidget(parent)
{
    /* Left viewlet */
    leftViewlet = new ViewletView(parent, FPOLayout);
    leftViewlet->leftVSet(parent, FPOLayout);

    /* Right viewlet */
    rightViewlet = new ViewletView(parent, FPOLayout);
    rightViewlet->defaultVSet(parent, FPOLayout);

    /* Default viewlet */
    defaultViewlet = new ViewletView(parent, FPOLayout);
    defaultViewlet->defaultVSet(parent, FPOLayout);

    /* Test another viewlet */
    //anotherViewlet = new ViewletView(parent, FPOLayout);
}

} // END namespace gnc
