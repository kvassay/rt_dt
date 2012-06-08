#ifndef TABLECOMBOBOX_H
#define TABLECOMBOBOX_H

#include <QComboBox>

class TableComboBox : public QComboBox
{
    Q_OBJECT
private:
    int row;
    int column;
private slots:
    void itemChangedEvent(int paIndex);

public:
    explicit TableComboBox(int paRow, int paColumn, QWidget *parent = 0);

    int getRow() { return row; }
    void setRow(int paRow) { row = paRow; }
    int getColumn() { return column; }
    void setColumn(int paColumn) { column = paColumn; }

signals:
    void itemChanged(TableComboBox* paTableComboBox);
};

#endif // TABLECOMBOBOX_H
