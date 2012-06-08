#include "tablecombobox.h"

TableComboBox::TableComboBox(int paRow, int paColumn, QWidget *parent) :
    QComboBox(parent)
{
    row = paRow;
    column = paColumn;

    QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(itemChangedEvent(int)));
}

void TableComboBox::itemChangedEvent(int paIndex)
{
    emit itemChanged(this);
}
