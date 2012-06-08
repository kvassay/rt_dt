#ifndef DETAILSWIDGET_H
#define DETAILSWIDGET_H

#include "Core/System/component.h"
#include "Core/ImportanceIndices/componentmeasures.h"
#include "Core/ImportanceIndices/componentindex.h"

#include <QWidget>
#include <QTableWidget>

class DetailsWidget : public QWidget, IComponentIndexChangedEventHandler
{
    Q_OBJECT
private:
    Component *component;
    ComponentMeasures::Type measureType;
    ComponentIndex *importanceIndex;

    QTableWidget *table;

    void initTable();
    void setTableItem(int paRow, int paColumn, const ImportanceIndex *paImportanceIndex);

private slots:
    void emitTableColumnResized(int paColumn, int paOldSize, int paNewSize);
    void emitTableRowResized(int paRow, int paOldSize, int paNewSize);

public:
    DetailsWidget(Component &paComponent, ComponentMeasures::Type paMeasureType, QWidget *parent = 0);
    virtual ~DetailsWidget();

    void setColumnsBy(const DetailsWidget* paDetailsWidget);
    void setRowsBy(const DetailsWidget* paDetailsWidget);

    Component* getComponent() { return component; }
    ComponentMeasures::Type getComponentMeasureType() { return measureType; }
    int getVerticalStretch() { return 2 + component->getStatesCount(); }

    virtual void componentIndexChanged(Component &paComponent, ComponentIndex *paComponentIndex);

public slots:
    void close();
    void resizeTableColumn(int paColumn, int paNewSize);
    void resizeTableRow(int paRow, int paNewSize);

signals:
    void closed(DetailsWidget* paDetailsWidget);
    void tableColumnResized(DetailsWidget* paDetailsWidget, int paColumn, int paNewSize);
    void tableRowResized(DetailsWidget* paDetailsWidget, int paColumn, int paNewSize);
};

#endif // DETAILSWIDGET_H
