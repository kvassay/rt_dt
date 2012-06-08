#ifndef TABLESPINBOX_H
#define TABLESPINBOX_H

#include <QSpinBox>

class TableSpinBox : public QSpinBox
{
    Q_OBJECT
private:
    int row;
    int column;
private slots:
    void valueChangedEvent(int paValue);

public:
    explicit TableSpinBox(int paRow, int paColumn, QWidget *parent = 0);

    int getRow() { return row; }
    void setRow(int paRow) { row = paRow; }
    int getColumn() { return column; }
    void setColumn(int paColumn) { column = paColumn; }

signals:
    void valueChanged(TableSpinBox* paTableSpinBox);
};

#endif // TABLESPINBOX_H
