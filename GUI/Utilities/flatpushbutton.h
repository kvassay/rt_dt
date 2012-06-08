#ifndef FLATPUSHBUTTON_H
#define FLATPUSHBUTTON_H

#include <QPushButton>

class FlatPushButton : public QPushButton
{
    Q_OBJECT

protected:
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);

public:
    explicit FlatPushButton(QWidget *parent = 0);
};

#endif // FLATPUSHBUTTON_H
