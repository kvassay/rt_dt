#include "flatpushbutton.h"

FlatPushButton::FlatPushButton(QWidget *parent) :
    QPushButton(parent)
{
    setFlat(true);
}

void FlatPushButton::enterEvent(QEvent *e)
{
    setFlat(false);
    QPushButton::enterEvent(e);
}

void FlatPushButton::leaveEvent(QEvent *e)
{
    setFlat(true);
    QPushButton::leaveEvent(e);
}
