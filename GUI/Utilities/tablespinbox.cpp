#include "tablespinbox.h"

TableSpinBox::TableSpinBox(int paRow, int paColumn, QWidget *parent) :
    QSpinBox(parent)
{
    row = paRow;
    column = paColumn;

    QObject::connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedEvent(int)));
}

void TableSpinBox::valueChangedEvent(int paValue)
{
    emit valueChanged(this);
}
