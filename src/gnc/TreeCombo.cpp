#include <QtGui>
#include "TreeCombo.hpp"
namespace gnc
{
        TreeComboBox::TreeComboBox(QWidget* parent = 0)
        {
                QTreeView* v = new QTreeView(this);
                setView(v);
                v->header()->hide();
                v->viewport()->installEventFilter(this);
        }

        bool TreeComboBox::eventFilter(QObject* object, QEvent* event)
        {
                if (event->type() == QEvent::MouseButtonPress && object == view()->viewport())
                {
                        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                        QModelIndex index = view()->indexAt(mouseEvent->pos());
                        if (!view()->visualRect(index).contains(mouseEvent->pos()))
                                skipNextHide = true;
                }
                return false;
        }

        void TreeComboBox::showPopup()
        {
                setRootModelIndex(QModelIndex());
                QComboBox::showPopup();
        }

        void TreeComboBox::hidePopup()
        {
                setRootModelIndex(view()->currentIndex().parent());
                setCurrentIndex(view()->currentIndex().row());
                if (skipNextHide)
                        skipNextHide = false;
                else
                        QComboBox::hidePopup();
        }
}
