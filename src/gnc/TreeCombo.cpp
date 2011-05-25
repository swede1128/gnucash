#include <QtGui>

class TreeComboBox : public QComboBox
{
public:
        TreeComboBox(QWidget* parent = 0) : QComboBox(parent), skipNextHide(false)
        {
                QTreeView* v = new QTreeView(this);
                setView(v);
                v->header()->hide();
                v->viewport()->installEventFilter(this);
        }

        bool eventFilter(QObject* object, QEvent* event)
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

        virtual void showPopup()
        {
                setRootModelIndex(QModelIndex());
                QComboBox::showPopup();
        }

        virtual void hidePopup()
        {
                setRootModelIndex(view()->currentIndex().parent());
                setCurrentIndex(view()->currentIndex().row());
                if (skipNextHide)
                        skipNextHide = false;
                else
                        QComboBox::hidePopup();
        }

private:
        bool skipNextHide;
};
