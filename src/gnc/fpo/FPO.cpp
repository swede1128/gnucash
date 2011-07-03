#include "FPO.hpp"
#include "gnc/dashboard.hpp"
#include "gnc/mainwindow.hpp"

#include <QLabel>

namespace gnc
{

FPO::FPO(QWidget *parent) :
    QWidget(parent)
{
    /* For now, create one viewlet; hardcoded. */
    oneViewlet = new ViewletView(parent);
}

} // END namespace gnc
