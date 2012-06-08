#include "detailswidget.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>

#include "GUI/Utilities/flatpushbutton.h"
#include "GUI/guiconfig.h"

DetailsWidget::DetailsWidget(Component &paComponent, ComponentMeasures::Type paMeasureType, QWidget *parent) :
    QWidget(parent)
{
    component = &paComponent;
    measureType = paMeasureType;

    table = new QTableWidget(this);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    FlatPushButton *pomCloseBtn = new FlatPushButton(this);
    pomCloseBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    pomCloseBtn->setMaximumWidth(40);
    pomCloseBtn->setMaximumHeight(30);
    pomCloseBtn->setIcon(QIcon(":/icons/close"));

    QObject::connect(pomCloseBtn, SIGNAL(clicked()), this, SLOT(close()));

    QLabel *pomLabel = new QLabel((ComponentMeasures::toString(paMeasureType) + " (" + component->getName() + ")").c_str());
    QFont pomFont;
    pomFont.setBold(true);
    pomLabel->setFont(pomFont);

    QGridLayout *pomLayout = new QGridLayout();
    pomLayout->addWidget(pomLabel,0,0);
    pomLayout->addWidget(pomCloseBtn,0,1);
    pomLayout->setColumnStretch(0,9);
    pomLayout->setColumnStretch(1,1);

    QVBoxLayout *pomMainLayout = new QVBoxLayout(this);
    pomMainLayout->addLayout(pomLayout);
    pomMainLayout->addWidget(table);
    setLayout(pomMainLayout);

    QObject::connect(table->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(emitTableColumnResized(int,int,int)));
    QObject::connect(table->verticalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(emitTableRowResized(int,int,int)));

    initTable();

    Error pomError;
    ComponentIndex *pomComponentIndex = component->getComponentIndex(measureType, pomError);
    componentIndexChanged(*component, pomComponentIndex);

    component->addComponentIndexChangedEventHandler(measureType, this);
}

DetailsWidget::~DetailsWidget()
{
    component->removeComponentIndexChangedEventHandler(measureType, this);
}

void DetailsWidget::close()
{
    component->removeComponentIndexChangedEventHandler(measureType, this);
    emit closed(this);
}

void DetailsWidget::initTable()
{
    d_STATESCOUNT pomComponentStatesCount = component->getStatesCount();
    d_STATESCOUNT pomSystemStatesCount = component->getSystem()->getStatesCount();

    table->setRowCount(pomComponentStatesCount + 2);
    table->setColumnCount(pomSystemStatesCount + 2);

    QStringList pomVerticalHeaders;
    for (d_COMPONENTSCOUNT i = 0; i < pomComponentStatesCount; i++)
    {
        pomVerticalHeaders.append(QString::number(i));
    }
    pomVerticalHeaders.append(tr("Sum"));
    pomVerticalHeaders.append(tr("Avg"));

    QStringList pomHorizontalHeaders;
    for (d_COMPONENTSCOUNT i = 0; i < pomSystemStatesCount; i++)
    {
        pomHorizontalHeaders.append(QString::number(i));
    }
    pomHorizontalHeaders.append(tr("Sum"));
    pomHorizontalHeaders.append(tr("Avg"));

    table->setVerticalHeaderLabels(pomVerticalHeaders);
    table->setHorizontalHeaderLabels(pomHorizontalHeaders);
}

void DetailsWidget::resizeTableColumn(int paColumn, int paNewSize)
{
    table->setColumnWidth(paColumn, paNewSize);
}

void DetailsWidget::resizeTableRow(int paRow, int paNewSize)
{
    table->setRowHeight(paRow, paNewSize);
}

void DetailsWidget::setColumnsBy(const DetailsWidget *paDetailsWidget)
{
    if (paDetailsWidget != NULL && paDetailsWidget != this)
    {
        QTableWidget *pomDetailsWidgetTable = paDetailsWidget->table;
        int pomColumnsCount = pomDetailsWidgetTable->columnCount();
        for (int i = 0; i < pomColumnsCount; i++)
        {
            table->setColumnWidth(i, pomDetailsWidgetTable->columnWidth(i));
        }
    }
}

void DetailsWidget::setRowsBy(const DetailsWidget *paDetailsWidget)
{
    if (paDetailsWidget != NULL && paDetailsWidget != this)
    {
        QTableWidget *pomDetailsWidgetTable = paDetailsWidget->table;
        int pomRowsCount = pomDetailsWidgetTable->rowCount();
        for (int i = 0; i < pomRowsCount; i++)
        {
            table->setRowHeight(i, pomDetailsWidgetTable->rowHeight(i));
        }
    }
}

void DetailsWidget::componentIndexChanged(Component &paComponent, ComponentIndex *paComponentIndex)
{
    if (&paComponent != component)
    {
        return;
    }

    table->clearContents();

    if (paComponentIndex == NULL)
    {
        return;
    }

    d_STATESCOUNT pomComponentStatesCount = component->getStatesCount();
    d_STATESCOUNT pomSystemStatesCount = component->getSystem()->getStatesCount();

    for (d_STATESCOUNT i = 0; i < pomComponentStatesCount; i++)
    {
        for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
        {
            const ImportanceIndex *pomIndexValue = paComponentIndex->getBaseIndex(i, j);
            setTableItem(i, j, pomIndexValue);
        }
    }

    for (d_STATESCOUNT i = 0; i < pomComponentStatesCount; i++)
    {
        const ImportanceIndex *pomIndexValue = paComponentIndex->getComponentStateIndexSum(i);
        setTableItem(i, pomSystemStatesCount, pomIndexValue);

        pomIndexValue = paComponentIndex->getComponentStateIndexAvg(i);
        setTableItem(i, pomSystemStatesCount + 1, pomIndexValue);
    }
    for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
    {
        const ImportanceIndex *pomIndexValue = paComponentIndex->getSystemStateIndexSum(j);
        setTableItem(pomComponentStatesCount, j, pomIndexValue);

        pomIndexValue = paComponentIndex->getSystemStateIndexAvg(j);
        setTableItem(pomComponentStatesCount + 1, j, pomIndexValue);
    }

    setTableItem(pomComponentStatesCount, pomSystemStatesCount, NULL);
    setTableItem(pomComponentStatesCount, pomSystemStatesCount + 1, NULL);
    setTableItem(pomComponentStatesCount + 1, pomSystemStatesCount, NULL);

    const ImportanceIndex *pomIndexValue = paComponentIndex->getMainIndexSum();
    setTableItem(pomComponentStatesCount + 1, pomSystemStatesCount, pomIndexValue);

    pomIndexValue = paComponentIndex->getMainIndexAvg();
    setTableItem(pomComponentStatesCount + 1, pomSystemStatesCount + 1, pomIndexValue);
}

void DetailsWidget::setTableItem(int paRow, int paColumn, const ImportanceIndex *paImportanceIndex)
{
    QTableWidgetItem *pomItem = table->item(paRow, paColumn);
    if (pomItem == NULL)
    {
        pomItem = new QTableWidgetItem();
        table->setItem(paRow, paColumn, pomItem);
    }
    if (paImportanceIndex != NULL)
    {
        pomItem->setText(QString::number(paImportanceIndex->getValue()));
        pomItem->setBackground(QBrush(GUIConfig::getTableUsedCellColor()));
        pomItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    else
    {
        pomItem->setText("");
        pomItem->setBackground(QBrush(GUIConfig::getTableNotUsedCellColor()));
        pomItem->setFlags(Qt::NoItemFlags);
    }
}

void DetailsWidget::emitTableColumnResized(int paColumn, int paOldSize, int paNewSize)
{
    emit tableColumnResized(this, paColumn, paNewSize);
}

void DetailsWidget::emitTableRowResized(int paRow, int paOldSize, int paNewSize)
{
    emit tableRowResized(this, paRow, paNewSize);
}


