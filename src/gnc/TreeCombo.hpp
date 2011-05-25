#ifndef TREECOMBO_HPP
#define TREECOMBO_HPP

#include <QComboBox>

namespace gnc
{

class TreeComboBox : public QComboBox
{
    //Q_OBJECT
public:
    //explicit TreeCombo(QComboBox parent = 0);
    //TreeCombo(QWidget* parent = 0);
    explicit TreeComboBox(QWidget* parent);
    bool eventFilter(QObject* object, QEvent* event);
    virtual void showPopup();
    virtual void hidePopup();

    //bool eventFilter(QObject* object, QEvent* event);

signals:

public slots:
private:
    bool skipNextHide;
    //bool TreeCombo::skipNextHide;
};

} // END namespace gnc

#endif // TREECOMBO_HPP
